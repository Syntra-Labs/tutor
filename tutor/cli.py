import argparse
import json
import os
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

from tutor import __version__
from tutor.catalog import ProblemCatalog
from tutor.runner import TestRunner
from tutor.scoreboard import (
    LEADERBOARD_FILE,
    export_static_site_data,
    load_leaderboard,
    merge_scoreboard_entries,
    verify_pr_isolation,
)
from tutor.state import get_config, get_progress, update_config
from tutor.sync import SyncManager
from tutor.workspace import WorkspaceManager

# ANSI formatting helpers
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
CYAN = "\033[96m"
MAGENTA = "\033[95m"
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

    if getattr(args, "track", None):
        q = args.track.lower()
        problems = [
            p for p in problems
            if q in p.get("track", "").lower() or q in p.get("category", "").lower()
        ]

    if getattr(args, "unlocked", False):
        problems = [p for p in problems if p.get("unlocked", True)]

    print(f"\n{BOLD}{CYAN}=== SYNTRA TUTOR — ATOMIC PROBLEM CATALOG ({len(problems)} problems) ==={RESET}\n")
    header = f"{'STATUS':<12} {'ID':<32} {'DIFF':<8} {'PREREQS':<16} {'TITLE'}"
    print(f"{BOLD}{header}{RESET}")
    print("-" * 108)

    current_track = None
    for p in problems:
        track = p.get("track", p.get("category", "Systems"))
        if track != current_track:
            current_track = track
            print(f"\n{BOLD}{MAGENTA}▸ {current_track}{RESET}")

        solved = p.get("solved", False)
        unlocked = p.get("unlocked", True)
        if solved:
            status = f"{GREEN}✓ SOLVED  {RESET}"
        elif unlocked:
            status = f"{CYAN}○ READY   {RESET}"
        else:
            status = f"{DIM}🔒 LOCKED {RESET}"

        pid = p.get("id", "")
        diff = p.get("difficulty", "Medium")
        diff_color = GREEN if diff == "Easy" else (YELLOW if diff == "Medium" else RED)
        deps = p.get("depends_on", [])
        dep_short = ",".join(d.split("-")[0] for d in deps) if deps else "—"
        title = p.get("title", pid)

        print(f"{status} {BOLD}{pid:<32}{RESET} {diff_color}{diff:<8}{RESET} {DIM}{dep_short:<16}{RESET} {title}")

    print("\n" + "-" * 108)
    solved_cnt = sum(1 for p in problems if p.get("solved", False))
    unlocked_cnt = sum(1 for p in problems if p.get("unlocked", True) and not p.get("solved", False))
    print(
        f"{GREEN}✓ {solved_cnt} Solved{RESET}  |  {CYAN}○ {unlocked_cnt} Ready to Pick{RESET}  |  "
        f"{DIM}Use 'tutor pick <id>', 'tutor test <id>', 'tutor dag', or 'tutor scoreboard'{RESET}\n"
    )
    return 0


def cmd_dag(args):
    """Displays the Prerequisite Skill-Tree DAG grouped by Track."""
    catalog = ProblemCatalog()
    problems = catalog.list_problems()
    prob_map = {p["id"]: p for p in problems}

    # Build children map
    children_map = {p["id"]: [] for p in problems}
    for p in problems:
        for dep in p.get("depends_on", []):
            if dep in children_map:
                children_map[dep].append(p["id"])

    print(f"\n{BOLD}{CYAN}=== PREREQUISITE SKILL-TREE DAG ({len(problems)} PROBLEMS) ==={RESET}\n")
    current_track = None
    for p in problems:
        track = p.get("track", p.get("category", "Systems"))
        if track != current_track:
            current_track = track
            print(f"\n{BOLD}{MAGENTA}━━━ {current_track} ━━━{RESET}")

        pid = p["id"]
        num = pid.split("-")[0]
        solved = p.get("solved", False)
        unlocked = p.get("unlocked", True)
        icon = f"{GREEN}✓{RESET}" if solved else (f"{CYAN}○{RESET}" if unlocked else f"{DIM}🔒{RESET}")

        deps = [d.split("-")[0] for d in p.get("depends_on", [])]
        unlocks = [c.split("-")[0] for c in children_map.get(pid, [])]
        dep_str = f"requires [{', '.join(deps)}]" if deps else "root primitive"
        unlock_str = f" ──► unlocks [{', '.join(unlocks)}]" if unlocks else ""

        print(f"  {icon} {BOLD}[{num}]{RESET} {p.get('title', pid):<48} {DIM}({dep_str}{unlock_str}){RESET}")
    print()
    return 0


def cmd_pick(args):
    catalog = ProblemCatalog()
    problem = catalog.get_problem(args.problem_id)
    if not problem:
        print(f"{RED}Error: Problem '{args.problem_id}' not found. Run 'tutor list' to see available problems.{RESET}")
        return 1

    if not problem.get("unlocked", True) and not getattr(args, "force", False):
        missing = ", ".join(problem.get("missing_deps", []))
        print(
            f"\n{YELLOW}⚠ Prerequisite Check:{RESET} {BOLD}{problem['id']}{RESET} builds on unsolved prerequisite(s): "
            f"{BOLD}{missing}{RESET}\n"
            f"  Solve those first to unlock this node naturally, or pass {CYAN}--force{RESET} to scaffold anyway.\n"
        )
        return 1

    config = get_config()
    lang = args.lang or (problem.get("languages") or [config.get("default_lang", "cpp")])[0]

    workspace = WorkspaceManager()
    solution_path, readme_path = workspace.scaffold(problem, lang=lang)

    print(f"\n{GREEN}✓ Prepared workspace for {BOLD}{problem.get('title', problem['id'])}{RESET}")
    if problem.get("depends_on"):
        print(f"  • Prereqs  : {DIM}{', '.join(problem['depends_on'])}{RESET}")
    print(f"  • Spec     : {CYAN}{readme_path}{RESET}")
    print(f"  • Solution : {BOLD}{GREEN}{solution_path}{RESET}\n")

    if not args.no_edit:
        editor = os.environ.get("EDITOR", "nvim")
        print(f"{DIM}Opening {editor} {solution_path}...{RESET}\n")
        os.system(f"{editor} '{solution_path}'")

    return 0


def _resolve_target_problem_id(explicit_id):
    if explicit_id:
        return explicit_id
    cwd = Path.cwd()
    if (cwd / "README.md").exists() and (cwd / "problem.json").exists():
        return cwd.name
    config = get_config()
    ws_root = Path(config["workspace_dir"])
    if ws_root.exists():
        dirs = [d for d in ws_root.iterdir() if d.is_dir()]
        if dirs:
            dirs.sort(key=lambda d: d.stat().st_mtime, reverse=True)
            return dirs[0].name
    return None


def cmd_test(args):
    catalog = ProblemCatalog()
    target_id = _resolve_target_problem_id(args.problem_id)

    if not target_id:
        print(f"{RED}Error: Please specify problem ID to test (e.g. 'tutor test 002'){RESET}")
        return 1

    problem = catalog.get_problem(target_id)
    if not problem:
        print(f"{RED}Error: Problem '{target_id}' not found.{RESET}")
        return 1

    config = get_config()
    lang = args.lang or (problem.get("languages") or [config.get("default_lang", "cpp")])[0]
    sub_dir = Path(args.submission_dir) if getattr(args, "submission_dir", None) else None

    print(f"\n⚡ Running locked test harness for {BOLD}{problem.get('title', problem['id'])}{RESET} [{lang}]...")
    runner = TestRunner()
    success, log, runtime_ms, metrics = runner.run_test(problem, lang=lang, submission_dir=sub_dir)

    print("\n" + "=" * 68)
    print(log)
    print("=" * 68)

    testcases = metrics.get("testcases") or []
    if testcases:
        print(f"\n{BOLD}Testcases ({len(testcases)} evaluated):{RESET}")
        for idx, tc in enumerate(testcases, 1):
            passed = tc.get("passed", False)
            mark = f"{GREEN}✓ PASS{RESET}" if passed else f"{RED}✗ FAIL{RESET}"
            inp = tc.get("input", "")
            out = tc.get("output", "")
            exp = tc.get("expected", "")
            print(f"  Case {idx}: {mark}  {CYAN}{inp}{RESET}")
            if not passed:
                print(f"    • Actual Output:   {RED}{out}{RESET}")
                print(f"    • Expected Output: {GREEN}{exp}{RESET}")

    if success:
        vec = metrics.get("scoreboard") or [100.0, 0.0, 0]
        print(f"\n{GREEN}{BOLD}✓ ALL TESTS PASSED!{RESET} ({runtime_ms:.2f}ms)")
        print(
            f"  {CYAN}Autolab Scoreboard Metrics:{RESET} "
            f"Score={BOLD}{vec[0]:.1f}{RESET} | "
            f"Cycles/Op={BOLD}{vec[1]:.2f}{RESET} | "
            f"Code Bytes={BOLD}{vec[2]} B{RESET}"
        )
        print(f"{GREEN}Problem marked as SOLVED in your local ledger.{RESET}\n")
        return 0
    else:
        print(f"\n{RED}{BOLD}✗ TESTS FAILED.{RESET} (Autolab scoreboard entry suppressed until 100% pass)\n")
        return 1


def cmd_submit(args):
    """
    Runs local verification and copies the passing solution into `submissions/<user>/<problem_id>/solution.hpp`
    so the student can open a PR to appear on the public GitHub Pages Autolab Scoreboard.
    """
    catalog = ProblemCatalog()
    target_id = _resolve_target_problem_id(args.problem_id)
    if not target_id:
        print(f"{RED}Error: Please specify problem ID to submit (e.g. 'tutor submit 002 --user <github_user>'){RESET}")
        return 1

    problem = catalog.get_problem(target_id)
    if not problem:
        print(f"{RED}Error: Problem '{target_id}' not found.{RESET}")
        return 1

    config = get_config()
    lang = args.lang or (problem.get("languages") or [config.get("default_lang", "cpp")])[0]
    user = args.user or os.environ.get("GITHUB_ACTOR") or os.environ.get("USER", "student")

    runner = TestRunner()
    success, log, runtime_ms, metrics = runner.run_test(problem, lang=lang)
    if not success:
        print(f"{RED}✗ Cannot submit '{problem['id']}': solution failed local verification.{RESET}\n{log}")
        return 1

    ws_prob_dir = Path(config["workspace_dir"]) / problem["id"]
    sol_src = ws_prob_dir / ("solution.sh" if lang == "sh" else "solution.hpp")
    if not sol_src.exists():
        print(f"{RED}Error: Solution file not found at {sol_src}{RESET}")
        return 1

    dest_dir = REPO_ROOT / "submissions" / user / problem["id"]
    dest_dir.mkdir(parents=True, exist_ok=True)
    shutil.copy2(sol_src, dest_dir / "solution.hpp")

    # Also record on local leaderboard if --local-board is passed
    if getattr(args, "update_board", False):
        merge_scoreboard_entries(
            [
                {
                    "user": user,
                    "problem_id": problem["id"],
                    "lang": lang,
                    "score": metrics["score"],
                    "scoreboard": metrics["scoreboard"],
                    "submitted_at": datetime.now(timezone.utc).isoformat(),
                }
            ]
        )

    vec = metrics["scoreboard"]
    print(f"\n{GREEN}{BOLD}✓ Staged verified submission for {problem['id']}!{RESET}")
    print(f"  • Path       : {CYAN}{dest_dir / 'solution.hpp'}{RESET}")
    print(f"  • Scoreboard : Score={vec[0]:.1f}, Cycles/Op={vec[1]:.2f}, Bytes={vec[2]} B")
    print(f"  • Next Step  : {DIM}git add submissions/{user}/{problem['id']} && git commit -m 'solve {problem['id']}' && git push{RESET}\n")
    return 0


def cmd_scoreboard(args):
    """Displays the Autolab Scoreboard (global standings or per-problem multi-metric table)."""
    board = load_leaderboard()
    catalog = ProblemCatalog()

    if getattr(args, "problem", None):
        prob = catalog.get_problem(args.problem)
        if not prob:
            print(f"{RED}Error: Problem '{args.problem}' not found.{RESET}")
            return 1
        pid = prob["id"]
        rows = board.get("problems", {}).get(pid, [])
        colspec = prob.get("colspec", [])
        col_headers = [c["hdr"] for c in colspec]

        print(f"\n{BOLD}{CYAN}=== AUTOLAB SCOREBOARD: {prob.get('title', pid)} ({pid}) ==={RESET}")
        sort_desc = ", ".join(f"{c['hdr']} ({'ASC' if c.get('asc') else 'DESC'})" for c in colspec)
        print(f"{DIM}Sort Order: {sort_desc}{RESET}\n")

        hdr = f"{'RANK':<6} {'USER':<20} " + " ".join(f"{h:<14}" for h in col_headers) + f" {'SUBMITTED AT'}"
        print(f"{BOLD}{hdr}{RESET}")
        print("-" * 86)
        if not rows:
            print(f"{DIM}  (No passing submissions recorded yet for {pid}){RESET}")
        for r in rows:
            vec = r.get("scoreboard") or []
            cols_str = " ".join(f"{str(v):<14}" for v in vec)
            print(f"#{r['rank']:<5} {BOLD}{r['user']:<20}{RESET} {cols_str} {DIM}{r.get('submitted_at', '')[:19]}{RESET}")
        print()
        return 0

    # Global leaderboard
    global_rows = board.get("global_rankings", [])
    print(f"\n{BOLD}{CYAN}=== SYNTRA TUTOR — GLOBAL AUTOLAB SCOREBOARD ==={RESET}\n")
    hdr = f"{'RANK':<6} {'USER':<22} {'SOLVED':<10} {'RANK #1s':<10} {'TOTAL SCORE':<14} {'TOTAL CYCLES':<14} {'TOTAL BYTES'}"
    print(f"{BOLD}{hdr}{RESET}")
    print("-" * 92)
    if not global_rows:
        print(f"{DIM}  (No submissions recorded yet){RESET}")
    for r in global_rows:
        print(
            f"#{r['rank']:<5} {BOLD}{r['user']:<22}{RESET} "
            f"{GREEN}{r['solved_count']:<10}{RESET} "
            f"{YELLOW}{r['first_place_count']:<10}{RESET} "
            f"{r['total_score']:<14.1f} "
            f"{r['total_cycles']:<14.2f} "
            f"{r['total_bytes']} B"
        )
    print()
    return 0


def cmd_ci_grade_pr(args):
    """
    Workflow 1 (Untrusted PR Sandbox):
    1. Verifies that all changed files in the PR are strictly under `submissions/<user>/`.
    2. Grades every modified problem submission using the locked `problems/<id>/cpp/harness.cpp`.
    3. Writes verified `scoreboard_entries.json` for Workflow 2 (`workflow_run`) to consume.
    """
    user = args.user
    changed_files = [f for f in args.changed_files if f.strip()]

    ok, msg = verify_pr_isolation(changed_files, user)
    if not ok:
        print(f"{RED}✗ {msg}{RESET}")
        return 1
    print(f"{GREEN}✓ PR Isolation Verified: {msg}{RESET}")

    # Identify unique problem IDs under submissions/<user>/<problem_id>/
    prob_ids = sorted(
        {
            Path(f).parts[2]
            for f in changed_files
            if len(Path(f).parts) >= 3 and Path(f).parts[0] == "submissions" and Path(f).parts[1] == user
        }
    )

    catalog = ProblemCatalog()
    runner = TestRunner()
    entries = []
    all_passed = True

    for pid in prob_ids:
        prob = catalog.get_problem(pid)
        if not prob:
            print(f"{RED}✗ Unknown problem ID in submission: {pid}{RESET}")
            all_passed = False
            continue

        sub_dir = REPO_ROOT / "submissions" / user / prob["id"]
        print(f"⚡ Grading {user}/{prob['id']}...")
        success, log, runtime_ms, metrics = runner.run_test(prob, lang="cpp", submission_dir=sub_dir)
        print(log)
        if not success:
            print(f"{RED}✗ Submission failed for {prob['id']}{RESET}")
            all_passed = False
        else:
            vec = metrics["scoreboard"]
            print(f"{GREEN}✓ Passed {prob['id']}: Score={vec[0]}, Cycles/Op={vec[1]}, Bytes={vec[2]}{RESET}")
            entries.append(
                {
                    "user": user,
                    "problem_id": prob["id"],
                    "lang": "cpp",
                    "score": metrics["score"],
                    "scoreboard": metrics["scoreboard"],
                    "submitted_at": datetime.now(timezone.utc).isoformat(),
                    "commit_sha": args.commit_sha or "pr",
                }
            )

    out_path = Path(args.output)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w", encoding="utf-8") as f:
        json.dump({"user": user, "all_passed": all_passed, "entries": entries}, f, indent=2)

    return 0 if all_passed else 1


def cmd_ci_merge_scoreboard(args):
    """
    Workflow 2 (Trusted `workflow_run` Scoreboard Updater):
    Reads verified `scoreboard_entries.json` artifact and merges into `docs/data/leaderboard.json`.
    """
    in_path = Path(args.input)
    if not in_path.exists():
        print(f"{RED}Error: Artifact file not found: {in_path}{RESET}")
        return 1

    with open(in_path, "r", encoding="utf-8") as f:
        payload = json.load(f)

    entries = payload.get("entries", [])
    if not entries:
        print(f"{YELLOW}No passing entries to merge.{RESET}")
        return 0

    board = merge_scoreboard_entries(entries)
    export_static_site_data()
    print(f"{GREEN}✓ Merged {len(entries)} entry/entries into {LEADERBOARD_FILE}. Global ranked users: {len(board.get('global_rankings', []))}{RESET}")
    return 0


def cmd_export_pages(args):
    cat_path, board_path = export_static_site_data()
    print(f"{GREEN}✓ Exported GitHub Pages data:{RESET}")
    print(f"  • Catalog     : {CYAN}{cat_path}{RESET}")
    print(f"  • Leaderboard : {CYAN}{board_path}{RESET}")
    return 0


def cmd_serve(args):
    """
    Serves the Neovim WASM Studio + Skill-Tree DAG + Autolab Scoreboard (`docs/`) locally
    and exposes `/api/judge` for sub-100ms local `g++ -O3 -std=c++20` execution.
    """
    import http.server
    import tempfile

    export_static_site_data()
    docs_dir = REPO_ROOT / "docs"
    port = int(getattr(args, "port", 8080))

    class TutorStudioHandler(http.server.SimpleHTTPRequestHandler):
        def __init__(self, *h_args, **h_kwargs):
            super().__init__(*h_args, directory=str(docs_dir), **h_kwargs)

        def do_POST(self):
            if self.path == "/api/judge":
                length = int(self.headers.get("Content-Length", "0"))
                raw = self.rfile.read(length).decode("utf-8")
                try:
                    req = json.loads(raw)
                    prob_id = req.get("problem_id", "")
                    code = req.get("code", "")
                    user = req.get("user") or os.environ.get("USER", "local-user")
                    catalog = ProblemCatalog()
                    prob = catalog.get_problem(prob_id)
                    if not prob:
                        self._send_json(404, {"ok": False, "log": f"Problem '{prob_id}' not found."})
                        return

                    with tempfile.TemporaryDirectory(prefix="tutor_web_") as tmp_dir:
                        sol_path = Path(tmp_dir) / "solution.hpp"
                        sol_path.write_text(code, encoding="utf-8")
                        runner = TestRunner()
                        success, log, runtime_ms, metrics = runner.run_test(
                            prob, lang="cpp", submission_dir=Path(tmp_dir)
                        )
                        if success:
                            from tutor.state import mark_solved
                            mark_solved(
                                prob["id"],
                                "cpp",
                                runtime_ms=runtime_ms,
                                cycles_per_op=metrics.get("cycles_per_op", 0.0),
                                code_bytes=metrics.get("code_bytes", 0),
                            )
                            if req.get("update_board", True):
                                merge_scoreboard_entries(
                                    [
                                        {
                                            "user": user,
                                            "problem_id": prob["id"],
                                            "lang": "cpp",
                                            "score": metrics["score"],
                                            "scoreboard": metrics["scoreboard"],
                                            "submitted_at": datetime.now(timezone.utc).isoformat(),
                                        }
                                    ]
                                )
                        self._send_json(
                            200,
                            {
                                "ok": success,
                                "log": log,
                                "runtime_ms": round(runtime_ms, 2),
                                "metrics": metrics,
                                "engine": "local-g++",
                            },
                        )
                except Exception as exc:
                    self._send_json(500, {"ok": False, "log": f"Server error: {exc}"})
            else:
                self.send_error(404)

        def _send_json(self, status_code: int, payload: dict):
            data = json.dumps(payload).encode("utf-8")
            self.send_response(status_code)
            self.send_header("Content-Type", "application/json")
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)

    host = getattr(args, "host", "127.0.0.1")
    print(f"\n{BOLD}{CYAN}=== SYNTRA TUTOR — NEOVIM WASM STUDIO & LOCAL JUDGE ==={RESET}")
    print(f"  • Studio URL : {GREEN}{BOLD}http://{host}:{port}{RESET}")
    print(f"  • Editor     : Genuine Neovim WebAssembly (:w, :Run, <Space>r, <F5>)")
    print(f"  • Judge      : Local g++ -O3 -std=c++20 (/api/judge) + Cloud Godbolt Fallback\n")
    with http.server.ThreadingHTTPServer((host, port), TutorStudioHandler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down studio server.")
    return 0


def cmd_pull(args):
    sync = SyncManager()
    remote = args.remote
    print("\n🔄 Pulling latest problem sets and test harnesses...")
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
    unlocked_count = sum(1 for p in problems if p.get("unlocked", True) and not p.get("solved", False))

    print(f"\n{BOLD}{CYAN}=== SYNTRA TUTOR STATUS ==={RESET}\n")
    print(f"  • Workspace : {config.get('workspace_dir')}")
    print(f"  • Upstream  : {config.get('remote_url')}")
    print(f"  • Node      : {config.get('distributed_node') or '(Local standalone + GitHub Pages Scoreboard)'}")
    print(f"  • Solved    : {GREEN}{solved_count} / {total} problems{RESET}")
    print(f"  • Unlocked  : {CYAN}{unlocked_count} ready to solve{RESET}\n")
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
        description="Code-Tutor — Sovereign Systems, Math & Geometry Problem Runner + Autolab Scoreboard",
    )
    parser.add_argument("-v", "--version", action="version", version=f"tutor {__version__}")
    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # list
    p_list = subparsers.add_parser("list", aliases=["ls"], help="List available problem sets and DAG unlock status")
    p_list.add_argument("--track", default=None, help="Filter by track name or number")
    p_list.add_argument("--unlocked", action="store_true", help="Show only unlocked problems")
    p_list.set_defaults(func=cmd_list)

    # dag / tree
    p_dag = subparsers.add_parser("dag", aliases=["tree"], help="Visualize the Prerequisite Skill-Tree DAG")
    p_dag.set_defaults(func=cmd_dag)

    # pick
    p_pick = subparsers.add_parser("pick", help="Scaffold and open a problem solution in editor")
    p_pick.add_argument("problem_id", help="Problem ID (e.g. 002, 2, or 002-hypervector-hamming)")
    p_pick.add_argument("--lang", default=None, help="Implementation language (cpp, rust, python)")
    p_pick.add_argument("--no-edit", action="store_true", help="Do not launch editor automatically")
    p_pick.add_argument("--force", action="store_true", help="Bypass prerequisite lock check")
    p_pick.set_defaults(func=cmd_pick)

    # test
    p_test = subparsers.add_parser("test", help="Execute locked test harness and compute Autolab scoreboard metrics")
    p_test.add_argument("problem_id", nargs="?", default=None, help="Problem ID (optional if inside workspace)")
    p_test.add_argument("--lang", default=None, help="Implementation language")
    p_test.add_argument("--submission-dir", default=None, help="Explicit directory containing solution to grade")
    p_test.set_defaults(func=cmd_test)

    # submit
    p_sub = subparsers.add_parser("submit", help="Verify and stage solution into submissions/<user>/<id>/ for PR")
    p_sub.add_argument("problem_id", nargs="?", default=None, help="Problem ID")
    p_sub.add_argument("--user", default=None, help="GitHub username for scoreboard attribution")
    p_sub.add_argument("--lang", default=None, help="Implementation language")
    p_sub.add_argument("--update-board", action="store_true", help="Also merge directly into local leaderboard.json")
    p_sub.set_defaults(func=cmd_submit)

    # scoreboard
    p_board = subparsers.add_parser("scoreboard", aliases=["board"], help="Display Autolab Scoreboard rankings")
    p_board.add_argument("-p", "--problem", default=None, help="Show multi-metric scoreboard for a specific problem")
    p_board.set_defaults(func=cmd_scoreboard)

    # export-pages
    p_exp = subparsers.add_parser("export-pages", help="Export catalog.json and leaderboard.json for GitHub Pages")
    p_exp.set_defaults(func=cmd_export_pages)

    # serve
    p_srv = subparsers.add_parser("serve", aliases=["web", "studio"], help="Launch Neovim WASM Studio + Local C++20 Judge in browser")
    p_srv.add_argument("-p", "--port", type=int, default=8080, help="HTTP port (default: 8080)")
    p_srv.add_argument("--host", default="127.0.0.1", help="Host interface to bind to (default: 127.0.0.1 for local security)")
    p_srv.set_defaults(func=cmd_serve)

    # ci-grade-pr
    p_cigrade = subparsers.add_parser("ci-grade-pr", help="CI Workflow 1: Verify PR isolation and grade submissions")
    p_cigrade.add_argument("--user", required=True, help="GitHub actor username")
    p_cigrade.add_argument("--commit-sha", default="", help="Commit SHA")
    p_cigrade.add_argument("--output", default="scoreboard_entries.json", help="Output JSON artifact path")
    p_cigrade.add_argument("changed_files", nargs="*", help="List of files modified in the PR")
    p_cigrade.set_defaults(func=cmd_ci_grade_pr)

    # ci-merge-scoreboard
    p_cimerge = subparsers.add_parser("ci-merge-scoreboard", help="CI Workflow 2: Merge verified artifact into leaderboard.json")
    p_cimerge.add_argument("--input", required=True, help="Path to scoreboard_entries.json artifact")
    p_cimerge.set_defaults(func=cmd_ci_merge_scoreboard)

    # pull
    p_pull = subparsers.add_parser("pull", aliases=["sync"], help="Pull updated problem sets & tests from remote")
    p_pull.add_argument("remote", nargs="?", default=None, help="Optional remote repository URL")
    p_pull.set_defaults(func=cmd_pull)

    # connect
    p_conn = subparsers.add_parser("connect", help="Connect to an optional distributed cluster/node")
    p_conn.add_argument("endpoint", help="Distributed node endpoint")
    p_conn.set_defaults(func=cmd_connect)

    # slice
    p_slice = subparsers.add_parser("slice", help="Clean-room contract slicer: extract problem stub from real code")
    p_slice.add_argument("file", help="Source file to slice (C++, Rust, Python, Lua)")
    p_slice.add_argument("-o", "--output", help="Output file path for stub")
    p_slice.set_defaults(func=cmd_slice)

    # lint
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
