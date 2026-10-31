import os
import sys
import time
import subprocess
from pathlib import Path
from typing import Dict, Any, Tuple

from tutor.state import get_config, mark_solved

class TestRunner:
    """Executes local test harnesses against user solutions."""

    def __init__(self):
        self.config = get_config()
        self.workspace_root = Path(self.config["workspace_dir"])

    def run_test(self, problem: Dict[str, Any], lang: str = "cpp") -> Tuple[bool, str, float]:
        """
        Compiles and executes the test harness for a problem in workspace.
        Returns (success: bool, output_log: str, runtime_ms: float).
        """
        prob_id = problem["id"]
        prob_dir = self.workspace_root / prob_id

        if not prob_dir.exists():
            return False, f"Problem directory not found in workspace: {prob_dir}", 0.0

        start_time = time.time()
        success = False
        log = ""

        # Determine test command based on files present in problem directory
        if (prob_dir / "Makefile").exists():
            cmd = ["make", "test"]
        elif (prob_dir / "Cargo.toml").exists():
            cmd = ["cargo", "test"]
        elif (prob_dir / "test_solution.py").exists():
            cmd = [sys.executable, "-m", "unittest", "test_solution.py"]
        elif (prob_dir / "harness.cpp").exists():
            # Direct compile & run
            bin_path = prob_dir / "harness_bin"
            compile_cmd = [
                "g++", "-O3", "-std=c++20", "-Wall", "-Wextra",
                "-I.", str(prob_dir / "harness.cpp"), "-o", str(bin_path)
            ]
            c_proc = subprocess.run(compile_cmd, cwd=str(prob_dir), capture_output=True, text=True)
            if c_proc.returncode != 0:
                return False, f"Compilation Error:\n{c_proc.stderr}", 0.0
            cmd = [str(bin_path)]
        else:
            return False, "No valid test harness found (missing Makefile, Cargo.toml, or harness.cpp)", 0.0

        try:
            proc = subprocess.run(
                cmd,
                cwd=str(prob_dir),
                capture_output=True,
                text=True,
                timeout=15,
            )
            runtime_ms = (time.time() - start_time) * 1000.0
            log = proc.stdout + ("\n" + proc.stderr if proc.stderr else "")
            success = (proc.returncode == 0)

            if success:
                mark_solved(prob_id, lang, runtime_ms)

            return success, log.strip(), runtime_ms

        except subprocess.TimeoutExpired:
            return False, "Test timed out (> 15 seconds) — Possible infinite loop or deadlock.", 0.0
        except Exception as e:
            return False, f"Test execution failed: {e}", 0.0
