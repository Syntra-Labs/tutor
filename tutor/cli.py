import os
import sys
import shutil
import argparse
import subprocess
from pathlib import Path

from tutor import __version__
from tutor.catalog import ProblemCatalog
from tutor.workspace import WorkspaceManager
from tutor.runner import TestRunner
from tutor.sync import SyncManager
from tutor.state import get_config, get_progress, update_config

# ANSI formatting helpers
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
CYAN = "\033[96m"
BOLD = "\033[1m"
DIM = "\033[2m"
RESET = "\033[0m"

REPO_ROOT = Path(__file__).resolve().parent.parent
LGCC_BIN = REPO_ROOT / "bin" / "lgcc"

def cmd_list(args):
    catalog = ProblemCatalog()
    problems = catalog.list_problems()

    if not problems:
        print(f"{YELLOW}No problem sets found. Run 'tutor pull' to fetch problem sets.{RESET}")
        return 0

    print(f"\n{BOLD}{CYAN}=== SYNTRA TUTOR — PROBLEM CATALOG ==={RESET}\n")
    header = f"{'STATUS':<8} {'ID':<28} {'DIFFICULTY':<12} {'CATEGORY':<24} {'TITLE'}"
    print(f"{BOLD}{header}{RESET}")
    print("-" * 90)

    for p in problems:
        solved = p.get("solved", False)
        status = f"{GREEN}✓ SOLVED{RESET}" if solved else f"{DIM}○ TODO{RESET}  "
        pid = p.get("id", "")
        diff = p.get("difficulty", "Medium")
        diff_color = GREEN if diff == "Easy" else (YELLOW if diff == "Medium" else RED)
        cat = p.get("category", "Systems")
        title = p.get("title", pid)

        print(f"{status:<17} {BOLD}{pid:<28}{RESET} {diff_color}{diff:<12}{RESET} {cat:<24} {title}")

    print("-" * 90)
    print(f"{DIM}Use 'tutor pick <id>' to begin a problem or 'tutor test <id>' to verify.{RESET}\n")
    return 0

def cmd_pick(args):
    catalog = ProblemCatalog()
    problem = catalog.get_problem(args.problem_id)
    if not problem:
        print(f"{RED}Error: Problem '{args.problem_id}' not found. Run 'tutor list' to see available problems.{RESET}")
        return 1

    config = get_config()
    lang = args.lang or config.get("default_lang", "cpp")

    workspace = WorkspaceManager()
    solution_path, readme_path = workspace.scaffold(problem, lang=lang)

    print(f"\n{GREEN}✓ Prepared workspace for {BOLD}{problem.get('title', problem['id'])}{RESET}")
    print(f"  • Spec     : {CYAN}{readme_path}{RESET}")
    print(f"  • Solution : {BOLD}{GREEN}{solution_path}{RESET}\n")

    if not args.no_edit:
        editor = os.environ.get("EDITOR", "nvim")
        print(f"{DIM}Opening {editor} {solution_path}...{RESET}\n")
        os.system(f"{editor} '{solution_path}'")

    return 0

def cmd_test(args):
    catalog = ProblemCatalog()
    target_id = args.problem_id

    # If problem ID omitted, attempt to detect from current directory or last modified
    if not target_id:
        cwd = Path.cwd()
        if (cwd / "README.md").exists() and (cwd / "problem.json").exists():
            target_id = cwd.name
        else:
            config = get_config()
            ws_root = Path(config["workspace_dir"])
            if ws_root.exists():
                dirs = [d for d in ws_root.iterdir() if d.is_dir()]
                if dirs:
                    dirs.sort(key=lambda d: d.stat().st_mtime, reverse=True)
                    target_id = dirs[0].name

    if not target_id:
        print(f"{RED}Error: Please specify problem ID to test (e.g. 'tutor test 001-lockfree-ringbuffer'){RESET}")
        return 1

    problem = catalog.get_problem(target_id)
    if not problem:
        print(f"{RED}Error: Problem '{target_id}' not found.{RESET}")
        return 1

    config = get_config()
    lang = args.lang or config.get("default_lang", "cpp")

    print(f"\n⚡ Running tests for {BOLD}{problem.get('title', problem['id'])}{RESET} [{lang}]...")
    runner = TestRunner()
    success, log, runtime_ms = runner.run_test(problem, lang=lang)

    print("\n" + "=" * 60)
    print(log)
    print("=" * 60)

    if success:
        print(f"\n{GREEN}{BOLD}✓ ALL TESTS PASSED!{RESET} ({runtime_ms:.1f}ms)")
        print(f"{GREEN}Problem marked as SOLVED in your local ledger.{RESET}\n")
        return 0
    else:
        print(f"\n{RED}{BOLD}✗ TESTS FAILED.{RESET} Review compiler/assertion diagnostics above.\n")
        return 1

def cmd_pull(args):
    sync = SyncManager()
    remote = args.remote
    print(f"\n🔄 Pulling latest problem sets and test harnesses...")
    success, msg = sync.pull(custom_remote=remote)
    if success:
        print(f"{GREEN}{BOLD}✓ {msg}{RESET}\n")
        return 0
    else:
        print(f"{RED}{BOLD}✗ {msg}{RESET}\n")
        return 1

def cmd_connect(args):
    sync = SyncManager()
    success, msg = sync.connect_node(args.endpoint)
    print(f"\n{GREEN}{BOLD}✓ {msg}{RESET}\n")
    return 0

def cmd_status(args):
    catalog = ProblemCatalog()
    problems = catalog.list_problems()
    config = get_config()

    total = len(problems)
    solved_count = sum(1 for p in problems if p.get("solved", False))

    print(f"\n{BOLD}{CYAN}=== SYNTRA TUTOR STATUS ==={RESET}\n")
    print(f"  • Workspace : {config.get('workspace_dir')}")
    print(f"  • Upstream  : {config.get('remote_url')}")
    print(f"  • Node      : {config.get('distributed_node') or '(Local standalone mode)'}")
    print(f"  • Progress  : {GREEN}{solved_count} / {total} problems solved{RESET}\n")
    return 0

def cmd_slice(args):
    """Clean-room contract slicer powered by lgcc: strips function bodies to stubs while preserving signatures."""
    target_path = Path(args.file)
    if not target_path.exists():
        print(f"{RED}Error: File '{args.file}' not found.{RESET}")
        return 1

    bin_path = LGCC_BIN if LGCC_BIN.exists() else Path(shutil.which("lgcc") or "")
    if bin_path.exists():
        proc = subprocess.run([str(bin_path), "--skeletonize", str(target_path)], capture_output=True, text=True)
        if proc.returncode == 0:
            output = proc.stdout
        else:
            print(f"{RED}lgcc error:\n{proc.stderr}{RESET}")
            return 1
    else:
        print(f"{YELLOW}Warning: lgcc binary not found. Build via 'cargo build --release' in crates/lgcc.{RESET}")
        return 1

    if args.output:
        with open(args.output, "w") as f:
            f.write(output)
        print(f"{GREEN}✓ Wrote problem stub to {args.output}{RESET}")
    else:
        print(output)
    return 0

def cmd_lint(args):
    """Formal task linter & contract verifier powered by lgcc."""
    root = args.root or "."
    bin_path = LGCC_BIN if LGCC_BIN.exists() else Path(shutil.which("lgcc") or "")
    if bin_path.exists():
        proc = subprocess.run([str(bin_path), "--lint-tasks", root], capture_output=False)
        return proc.returncode
    else:
        print(f"{YELLOW}Warning: lgcc binary not found. Build via 'cargo build --release' in crates/lgcc.{RESET}")
        return 1

def main(argv=None):
    parser = argparse.ArgumentParser(
        prog="tutor",
        description="Code-Tutor — Sovereign Systems Programming Problem Runner & Logical Compiler",
    )
    parser.add_argument("-v", "--version", action="version", version=f"tutor {__version__}")
    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # list
    p_list = subparsers.add_parser("list", aliases=["ls"], help="List available problem sets and status")
    p_list.set_defaults(func=cmd_list)

    # pick
    p_pick = subparsers.add_parser("pick", help="Scaffold and open a problem solution in editor")
    p_pick.add_argument("problem_id", help="Problem ID (e.g. 001, 1, or 001-lockfree-ringbuffer)")
    p_pick.add_argument("--lang", default=None, help="Implementation language (cpp, rust, python)")
    p_pick.add_argument("--no-edit", action="store_true", help="Do not launch editor automatically")
    p_pick.set_defaults(func=cmd_pick)

    # test
    p_test = subparsers.add_parser("test", help="Execute local test harness against your solution")
    p_test.add_argument("problem_id", nargs="?", default=None, help="Problem ID (optional if inside workspace)")
    p_test.add_argument("--lang", default=None, help="Implementation language")
    p_test.set_defaults(func=cmd_test)

    # pull
    p_pull = subparsers.add_parser("pull", aliases=["sync"], help="Pull updated problem sets & tests from remote")
    p_pull.add_argument("remote", nargs="?", default=None, help="Optional remote repository URL")
    p_pull.set_defaults(func=cmd_pull)

    # connect
    p_conn = subparsers.add_parser("connect", help="Connect to an optional distributed cluster/node")
    p_conn.add_argument("endpoint", help="Distributed node endpoint (e.g. node.syntralabs.org:9000)")
    p_conn.set_defaults(func=cmd_connect)

    # slice (contract slicer)
    p_slice = subparsers.add_parser("slice", help="Clean-room contract slicer: extract problem stub from real code")
    p_slice.add_argument("file", help="Source file to slice (C++, Rust, Python, Lua)")
    p_slice.add_argument("-o", "--output", help="Output file path for stub")
    p_slice.set_defaults(func=cmd_slice)

    # lint (task contract linter)
    p_lint = subparsers.add_parser("lint", help="Lint task contracts and verification obligations")
    p_lint.add_argument("root", nargs="?", default=".", help="Root directory to scan")
    p_lint.set_defaults(func=cmd_lint)

    # status
    p_stat = subparsers.add_parser("status", help="Show workspace status and solved metrics")
    p_stat.set_defaults(func=cmd_status)

    args = parser.parse_args(argv)
    if not args.command:
        return cmd_list(args)

    return args.func(args)

if __name__ == "__main__":
    sys.exit(main())
