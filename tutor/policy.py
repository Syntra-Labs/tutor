"""
Syntra Tutor: Active Policy Enforcement Engine.

Executes and enforces the canonical policies defined in `policy/judge.rego`:
  - POL-000: Principle of Executable Policy as Single Source of Truth
  - POL-001: Submitter Path Isolation (submissions/<actor>/* only)
  - POL-002: Allowed Artifact Whitelist (solution.hpp or solution.sh only)
  - POL-003: Monolithic Leaderboard Modification Ban
  - POL-004: Problem Catalog & CI Infrastructure Lock
  - POL-005: Deterministic Scoring Metrics Contract (No volatile wall-clock runtime)
"""

from __future__ import annotations

import json
import shutil
import subprocess
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

POLICY_FILE = Path(__file__).resolve().parent.parent / "policy" / "judge.rego"


def evaluate_policy_opa(input_data: Dict[str, Any], policy_path: Path = POLICY_FILE) -> Optional[Tuple[bool, List[str]]]:
    """
    If `opa` binary is installed on the host or CI runner, evaluate via official OPA CLI.
    Returns (allowed, list_of_violations) or None if `opa` is not available.
    """
    opa_bin = shutil.which("opa")
    if not opa_bin or not policy_path.exists():
        return None

    try:
        proc = subprocess.run(
            [
                opa_bin,
                "eval",
                "--data",
                str(policy_path),
                "--input",
                "-",
                "data.tutor.judge.violation",
            ],
            input=json.dumps(input_data),
            capture_output=True,
            text=True,
            check=True,
        )
        res = json.loads(proc.stdout)
        results = res.get("result", [])
        violations = []
        if results and "expressions" in results[0]:
            raw_vals = results[0]["expressions"][0].get("value", [])
            violations = list(raw_vals) if isinstance(raw_vals, (list, set)) else []
        return (len(violations) == 0, violations)
    except Exception:
        return None


def evaluate_policy_native(input_data: Dict[str, Any]) -> Tuple[bool, List[str]]:
    """
    Native Python evaluation of the exact rules codified in `policy/judge.rego`.
    Guarantees 100% policy enforcement without requiring external binaries.
    """
    actor = input_data.get("actor", "")
    changed_files = input_data.get("changed_files", [])
    catalog_problems = input_data.get("catalog_problems", [])
    violations: List[str] = []

    for file in changed_files:
        # POL-001: Submitter Path Isolation
        expected_prefix = f"submissions/{actor}/"
        if not file.startswith(expected_prefix):
            violations.append(
                f"[POL-001 Path Isolation Breach]: File '{file}' is outside submissions/{actor}/. "
                "Submissions are strictly sandboxed to the contributor's own directory."
            )
            continue

        # POL-002: Allowed Artifact Whitelist
        if not (file.endswith("/solution.hpp") or file.endswith("/solution.sh")):
            violations.append(
                f"[POL-002 Invalid Artifact]: File '{file}' is not an allowed submission artifact. "
                "Submissions must strictly be 'solution.hpp' or 'solution.sh'."
            )

        # POL-003: Monolithic Leaderboard Modification Ban
        if file == "docs/data/leaderboard.json":
            violations.append(
                "[POL-003 Leaderboard Direct Edit]: 'docs/data/leaderboard.json' cannot be modified in a submission PR. "
                "Standings are recomputed post-merge by the automated Autolab pipeline."
            )

        # POL-004: Problem Catalog & CI Infrastructure Lock
        if file.startswith("problems/"):
            violations.append(
                f"[POL-004 Problem Specification Lock]: File '{file}' belongs to the core problem catalog and "
                "cannot be modified in a competitor submission PR."
            )
        if file.startswith(".github/"):
            violations.append(
                f"[POL-004 CI Infrastructure Lock]: File '{file}' defines grading infrastructure and "
                "cannot be modified in an unprivileged submission PR."
            )

    # POL-005: Deterministic Metrics Contract
    for prob in catalog_problems:
        colspec = prob.get("colspec", [])
        if colspec:
            primary_hdr = str(colspec[0].get("hdr", "")).lower()
            if any(term in primary_hdr for term in ["wall", "runtime_ms", "elapsed"]):
                violations.append(
                    f"[POL-005 Non-Deterministic Metric]: Problem '{prob.get('id')}' uses volatile wall-clock "
                    f"metric '{primary_hdr}' as primary sort key. Deterministic cycle counters or test results are required."
                )

    return (len(violations) == 0, violations)


def verify_submission_policy(
    changed_files: List[str],
    actor: str,
    catalog_problems: Optional[List[Dict[str, Any]]] = None,
) -> Tuple[bool, List[str]]:
    """
    Evaluates judge policies on the given PR diff and contributor handle.
    Prefers official OPA evaluator if present, otherwise falls back to native engine.
    """
    input_data = {
        "actor": actor,
        "changed_files": changed_files,
        "catalog_problems": catalog_problems or [],
    }

    # 1. Try official OPA runner
    opa_result = evaluate_policy_opa(input_data)
    if opa_result is not None:
        return opa_result

    # 2. Native fallback
    return evaluate_policy_native(input_data)
