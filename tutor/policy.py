"""
Syntra Tutor: Active Architecture-as-Code & Policy Enforcement Engine.

Executes and enforces the canonical policies defined in:
  - `policy/architecture.rego`:
      ARCH-001: Zero Markdown Invariant ("No Passive Documentation in Codebase")
      ARCH-002: Submitter Namespace Isolation (submissions/<actor>/* only)
      ARCH-003: Core Catalog & CI Infrastructure Immutability
      ARCH-004: Monolithic Leaderboard Modification Ban
      ARCH-005: Deterministic Benchmark Contract (Volatile Wall-Clock Ban)
  - `policy/judge.rego`: Active PR isolation and artifact whitelist
"""

from __future__ import annotations

import json
import shutil
import subprocess
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

REPO_ROOT = Path(__file__).resolve().parent.parent
POLICY_DIR = REPO_ROOT / "policy"
ARCH_POLICY_FILE = POLICY_DIR / "architecture.rego"
JUDGE_POLICY_FILE = POLICY_DIR / "judge.rego"


def scan_markdown_prohibitions(repo_root: Path = REPO_ROOT) -> List[Dict[str, str]]:
    """
    Scans the repository for passive Markdown documents, flagging them
    and assigning migration targets according to ARCH-001 in `policy/architecture.rego`.
    """
    flagged: List[Dict[str, str]] = []
    for p in sorted(repo_root.rglob("*.md")):
        if ".git" in p.parts:
            continue
        rel = str(p.relative_to(repo_root))
        if rel == "ROADMAP_MATH_AND_ML.md":
            target = "Convert to GitHub Project Milestone or Structured Schema"
        elif rel.endswith("PROBLEM_MANAGEMENT_SCALING_AUDIT.md"):
            target = "Convert to GitHub Architecture Issue"
        elif "problems/" in rel:
            target = "Embed 'spec' text directly inside problem.json ('description' field)"
        elif rel == "README.md":
            target = "Convert to minimal repo manifest or GitHub About description"
        else:
            target = "Convert to structured JSON/Rego policy or GitHub Issue"

        flagged.append(
            {
                "file": rel,
                "target": target,
                "violation": f"[ARCH-001 Markdown Prohibition]: File '{rel}' is forbidden. Migration target: {target}.",
            }
        )
    return flagged


def evaluate_policy_opa(
    input_data: Dict[str, Any],
    policy_path: Path = ARCH_POLICY_FILE,
    rule_query: str = "data.tutor.architecture.violations",
) -> Optional[Tuple[bool, List[str]]]:
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
                rule_query,
            ],
            input=json.dumps(input_data),
            capture_output=True,
            text=True,
            check=True,
        )
        res = json.loads(proc.stdout)
        results = res.get("result", [])
        violations: List[str] = []
        if results and "expressions" in results[0]:
            raw_vals = results[0]["expressions"][0].get("value", [])
            violations = list(raw_vals) if isinstance(raw_vals, (list, set)) else []
        return (len(violations) == 0, violations)
    except Exception:
        return None


def evaluate_policy_native(input_data: Dict[str, Any]) -> Tuple[bool, List[str]]:
    """
    Native Python evaluation of the exact rules codified in `policy/architecture.rego`.
    Guarantees 100% policy enforcement without requiring external binaries.
    """
    actor = input_data.get("actor", "")
    changed_files = input_data.get("changed_files", [])
    all_files = input_data.get("all_files", [])
    catalog_problems = input_data.get("catalog_problems", [])
    violations: List[str] = []

    # ARCH-001: Zero Markdown Prohibition
    for file in all_files:
        if file.endswith(".md"):
            if file.endswith("ROADMAP_MATH_AND_ML.md"):
                target = "Convert to GitHub Project Milestone or Structured Schema"
            elif file.endswith("PROBLEM_MANAGEMENT_SCALING_AUDIT.md"):
                target = "Convert to GitHub Architecture Issue"
            elif "problems/" in file:
                target = "Embed 'spec' text directly inside problem.json ('description' field)"
            elif file == "README.md":
                target = "Convert to minimal repo manifest or GitHub About description"
            else:
                target = "Convert to structured JSON/Rego policy or GitHub Issue"
            violations.append(
                f"[ARCH-001 Markdown Prohibition]: Passive file '{file}' is forbidden. Migration target: {target}."
            )

    # ARCH-002: Submitter Namespace Isolation
    for file in changed_files:
        expected_prefix = f"submissions/{actor}/"
        if not file.startswith(expected_prefix):
            violations.append(
                f"[ARCH-002 Namespace Breach]: Contributor @{actor} modified '{file}' outside their isolated directory 'submissions/{actor}/'."
            )
            continue

        if not (file.endswith("/solution.hpp") or file.endswith("/solution.sh")):
            violations.append(
                f"[ARCH-002 Invalid Artifact]: File '{file}' is not an allowed submission artifact. Submissions must strictly be 'solution.hpp' or 'solution.sh'."
            )

        if file == "docs/data/leaderboard.json":
            violations.append(
                "[ARCH-004 Leaderboard Direct Edit]: 'docs/data/leaderboard.json' cannot be edited directly. The board is aggregated post-merge."
            )

        if file.startswith("problems/"):
            violations.append(
                f"[ARCH-003 Catalog Tamper]: Core problem catalog '{file}' is immutable in submission PRs."
            )
        if file.startswith(".github/"):
            violations.append(
                f"[ARCH-003 Infrastructure Tamper]: CI workflow '{file}' is immutable in submission PRs."
            )

    # ARCH-005: Deterministic Metrics Contract
    known_pids = {p.get("id") for p in catalog_problems if p.get("id")}
    for prob in catalog_problems:
        pid = prob.get("id")
        colspec = prob.get("colspec", [])
        if colspec:
            primary_hdr = str(colspec[0].get("hdr", "")).lower()
            if any(term in primary_hdr for term in ["wall", "runtime_ms", "elapsed"]):
                violations.append(
                    f"[ARCH-005 Volatile Timing Ban]: Problem '{pid}' uses volatile metric '{primary_hdr}'. "
                    "Hardware cycles or testcase pass rate required."
                )

        # ARCH-006: Catalog Schema Completeness & Colspec Registration
        if not pid:
            violations.append("[ARCH-006 Incomplete Metadata]: Problem definition missing mandatory 'id' field.")
        if not colspec:
            violations.append(f"[ARCH-006 Missing Colspec]: Problem '{pid}' omits 'colspec' scoring vector definition.")
        if not prob.get("languages"):
            violations.append(f"[ARCH-006 Missing Languages]: Problem '{pid}' omits 'languages' whitelist.")

        # ARCH-007: Prerequisite Skill-Tree DAG Reference Integrity
        for dep in prob.get("depends_on", []):
            if dep not in known_pids:
                violations.append(
                    f"[ARCH-007 Broken DAG Edge]: Problem '{pid}' references nonexistent prerequisite '{dep}'."
                )

    return (len(violations) == 0, violations)


def verify_architecture_policy(
    changed_files: Optional[List[str]] = None,
    actor: str = "local-user",
    catalog_problems: Optional[List[Dict[str, Any]]] = None,
    check_all_markdown: bool = False,
) -> Tuple[bool, List[str]]:
    """
    Evaluates Architecture-as-Code policies defined in `policy/architecture.rego`.
    """
    all_files = []
    if check_all_markdown:
        all_files = [str(p.relative_to(REPO_ROOT)) for p in REPO_ROOT.rglob("*.md") if ".git" not in p.parts]

    input_data = {
        "actor": actor,
        "changed_files": changed_files or [],
        "all_files": all_files,
        "catalog_problems": catalog_problems or [],
    }

    opa_result = evaluate_policy_opa(input_data, policy_path=ARCH_POLICY_FILE, rule_query="data.tutor.architecture.violations")
    if opa_result is not None:
        return opa_result

    return evaluate_policy_native(input_data)
