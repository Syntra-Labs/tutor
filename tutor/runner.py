import json
import os
import re
import secrets
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path
from typing import Dict, Any, Optional, Tuple

from tutor.state import get_config, mark_solved


def count_effective_code_bytes(path: Path) -> int:
    """Counts non-whitespace, non-comment bytes in a source file for Autolab scoreboard tie-breaking."""
    if not path.exists() or not path.is_file():
        return 0
    try:
        text = path.read_text(encoding="utf-8", errors="ignore")
        # Strip C/C++/Rust block comments and line comments
        text = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
        text = re.sub(r"//[^\n]*", "", text)
        text = re.sub(r"#[^\n]*", "", text)
        compact = "".join(ch for ch in text if not ch.isspace())
        return len(compact.encode("utf-8"))
    except Exception:
        return 0


class TestRunner:
    """Executes locked test harnesses against user solutions locally or in CI."""

    def __init__(self):
        self.config = get_config()
        self.workspace_root = Path(self.config["workspace_dir"])

    def run_test(
        self,
        problem: Dict[str, Any],
        lang: str = "cpp",
        submission_dir: Optional[Path] = None,
    ) -> Tuple[bool, str, float, Dict[str, Any]]:
        """
        Compiles and executes the locked test harness for a problem against a user's solution.
        Returns (success: bool, output_log: str, runtime_ms: float, metrics: dict).
        """
        prob_id = problem["id"]
        locked_prob_dir = Path(problem["dir"]) / lang
        if not locked_prob_dir.exists():
            locked_prob_dir = Path(problem["dir"]) / "cpp"

        user_prob_dir = Path(submission_dir) if submission_dir else (self.workspace_root / prob_id)

        if not user_prob_dir.exists():
            return (
                False,
                f"Problem directory not found: {user_prob_dir}",
                0.0,
                {"score": 0.0, "scoreboard": None},
            )

        with tempfile.TemporaryDirectory(prefix=f"tutor_eval_{prob_id}_") as tmp_str:
            sandbox_dir = Path(tmp_str)

            # 1. Copy user files first
            for item in user_prob_dir.iterdir():
                if item.name == "harness_bin" or item.name.startswith(".tutor"):
                    continue
                dst = sandbox_dir / item.name
                if item.is_file():
                    shutil.copy2(item, dst)
                elif item.is_dir():
                    shutil.copytree(item, dst)

            # 2. Overwrite test harness from the locked problem catalog so user cannot tamper with tests
            if locked_prob_dir.exists():
                for item in locked_prob_dir.iterdir():
                    if item.name.startswith("solution_stub") or item.name.startswith("stub"):
                        continue
                    dst = sandbox_dir / item.name
                    if item.is_file():
                        shutil.copy2(item, dst)
                    elif item.is_dir():
                        if dst.exists():
                            shutil.rmtree(dst)
                        shutil.copytree(item, dst)

            # Measure solution code bytes
            sol_file = sandbox_dir / "solution.hpp"
            if not sol_file.exists():
                for cand in ["solution.sh", "solution.py", "solution.rs", "solution.h", "Makefile"]:
                    if (sandbox_dir / cand).exists():
                        sol_file = sandbox_dir / cand
                        break
            code_bytes = count_effective_code_bytes(sol_file)

            # Determine build & test command
            nonce = f"TUTOR_{secrets.token_hex(8)}"
            if (sandbox_dir / "harness.cpp").exists():
                # Inject tutor_check.hpp testcase decomposer so local runs have 100% parity with cloud judge
                tutor_check_src = Path(__file__).parent / "include" / "tutor_check.hpp"
                if tutor_check_src.exists():
                    shutil.copy2(tutor_check_src, sandbox_dir / "tutor_check.hpp")
                harness_file = sandbox_dir / "harness.cpp"
                h_code = harness_file.read_text(encoding="utf-8")
                if 'tutor_check.hpp' not in h_code:
                    h_code = re.sub(
                        r"#include\s+<cassert>",
                        '#include <cassert>\n#include "tutor_check.hpp"',
                        h_code,
                        count=1
                    )
                # Secure benchmark output against user stdout spoofing
                h_code = re.sub(r"SCOREBOARD_JSON\s*:", f"{nonce}:SCOREBOARD_JSON:", h_code)
                # Wrap int main() with real_main and emit completion sentinel
                main_wrapper = (
                    f'int _tutor_real_main();\n'
                    f'int main() {{ int r = _tutor_real_main(); if (r == 0) std::cout << "\\n{nonce}:DONE\\n"; return r; }}\n'
                    f'int _tutor_real_main() {{'
                )
                h_code = re.sub(
                    r"int\s+main\s*\(\s*\)\s*\{",
                    lambda _: main_wrapper,
                    h_code,
                    count=1
                )
                harness_file.write_text(h_code, encoding="utf-8")

                bin_path = sandbox_dir / "harness_bin"
                compile_cmd = [
                    "g++",
                    "-O3",
                    "-std=c++20",
                    "-Wall",
                    "-Wextra",
                    "-I.",
                    str(sandbox_dir / "harness.cpp"),
                    "-o",
                    str(bin_path),
                ]
                c_proc = subprocess.run(
                    compile_cmd, cwd=str(sandbox_dir), capture_output=True, text=True
                )
                if c_proc.returncode != 0:
                    return (
                        False,
                        f"Compilation Error:\n{c_proc.stderr}",
                        0.0,
                        {"score": 0.0, "scoreboard": None},
                    )
                cmd = [str(bin_path)]
            elif (sandbox_dir / "Makefile").exists():
                cmd = ["make", "test"]
            elif (sandbox_dir / "Cargo.toml").exists():
                cmd = ["cargo", "test", "--release"]
            elif (sandbox_dir / "test_solution.py").exists():
                cmd = [sys.executable, "-m", "unittest", "test_solution.py"]
            elif (sandbox_dir / "harness.sh").exists():
                cmd = ["/bin/sh", "harness.sh"]
            else:
                return (
                    False,
                    "No valid test harness found (missing Makefile, Cargo.toml, test_solution.py, harness.sh, or harness.cpp)",
                    0.0,
                    {"score": 0.0, "scoreboard": None},
                )

            try:
                start_ns = time.perf_counter_ns()
                proc = subprocess.run(
                    cmd,
                    cwd=str(sandbox_dir),
                    capture_output=True,
                    text=True,
                    timeout=15,
                )
                elapsed_ns = time.perf_counter_ns() - start_ns
                runtime_ms = elapsed_ns / 1_000_000.0
                log = proc.stdout + ("\n" + proc.stderr if proc.stderr else "")

                # Parse authenticated SCOREBOARD_JSON line emitted by harness benchmark and TESTCASE_JSON lines
                cycles_per_op = round(runtime_ms * 100.0, 2)
                clean_lines = []
                testcases = []
                done_received = (f"{nonce}:DONE" in log) if ((sandbox_dir / "harness.cpp").exists()) else True

                for line in log.splitlines():
                    if (sandbox_dir / "harness.cpp").exists() and f"{nonce}:SCOREBOARD_JSON:" in line:
                        try:
                            payload = json.loads(line.split(f"{nonce}:SCOREBOARD_JSON:", 1)[1].strip())
                            if "cycles_per_op" in payload:
                                cycles_per_op = round(float(payload["cycles_per_op"]), 3)
                        except Exception:
                            pass
                    elif not (sandbox_dir / "harness.cpp").exists() and line.startswith("SCOREBOARD_JSON:"):
                        try:
                            payload = json.loads(line.split("SCOREBOARD_JSON:", 1)[1].strip())
                            if "cycles_per_op" in payload:
                                cycles_per_op = round(float(payload["cycles_per_op"]), 3)
                        except Exception:
                            pass
                    elif line.startswith("TESTCASE_JSON:"):
                        try:
                            tc = json.loads(line.split("TESTCASE_JSON:", 1)[1].strip())
                            testcases.append(tc)
                        except Exception:
                            pass
                    elif (sandbox_dir / "harness.cpp").exists() and f"{nonce}:DONE" in line:
                        continue
                    else:
                        clean_lines.append(line)
                clean_log = "\n".join(clean_lines).strip()

                if not done_received:
                    success = False
                    clean_log = (clean_log + "\n[Security Error]: Test harness did not reach normal completion (exit(0) bypass or premature termination detected).").strip()
                else:
                    success = (proc.returncode == 0) and len(testcases) > 0 and all(c.get("passed", False) for c in testcases)

                if success:
                    score = 100.0
                    scoreboard_vec = [score, cycles_per_op, code_bytes]
                    if submission_dir is None:
                        mark_solved(
                            prob_id,
                            lang,
                            runtime_ms=runtime_ms,
                            cycles_per_op=cycles_per_op,
                            code_bytes=code_bytes,
                        )
                    metrics = {
                        "score": score,
                        "cycles_per_op": cycles_per_op,
                        "code_bytes": code_bytes,
                        "runtime_ms": round(runtime_ms, 3),
                        "scoreboard": scoreboard_vec,
                        "testcases": testcases,
                    }
                else:
                    # Autolab zero_all_scoreboards rule: failing submissions never enter the scoreboard
                    metrics = {
                        "score": 0.0,
                        "cycles_per_op": None,
                        "code_bytes": code_bytes,
                        "runtime_ms": round(runtime_ms, 3),
                        "scoreboard": None,
                        "testcases": testcases,
                    }

                return success, clean_log, runtime_ms, metrics

            except subprocess.TimeoutExpired:
                return (
                    False,
                    "Test timed out (> 15 seconds) — Possible infinite loop or deadlock.",
                    15000.0,
                    {"score": 0.0, "scoreboard": None},
                )
            except Exception as e:
                return (
                    False,
                    f"Test execution failed: {e}",
                    0.0,
                    {"score": 0.0, "scoreboard": None},
                )
