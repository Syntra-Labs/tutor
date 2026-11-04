import functools
import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

from tutor.catalog import DEFAULT_COLSPEC, ProblemCatalog, REPO_ROOT

DOCS_DATA_DIR = REPO_ROOT / "docs" / "data"
LEADERBOARD_FILE = DOCS_DATA_DIR / "leaderboard.json"
CATALOG_EXPORT_FILE = DOCS_DATA_DIR / "catalog.json"


def compare_autolab_entries(
    a: Dict[str, Any], b: Dict[str, Any], colspec: List[Dict[str, Any]]
) -> int:
    """
    Autolab-compatible multi-metric comparator:
    Compares two passing submission entries `a` and `b` column-by-column according to `colspec`.
    - If `asc: True`, smaller values rank higher (come first).
    - If `asc: False`, larger values rank higher (come first).
    - Ties are broken by earliest submission timestamp (`submitted_at`).
    """
    vec_a = a.get("scoreboard") or []
    vec_b = b.get("scoreboard") or []

    for idx, col in enumerate(colspec):
        asc = bool(col.get("asc", True))
        val_a = float(vec_a[idx]) if idx < len(vec_a) and vec_a[idx] is not None else (float("inf") if asc else float("-inf"))
        val_b = float(vec_b[idx]) if idx < len(vec_b) and vec_b[idx] is not None else (float("inf") if asc else float("-inf"))

        if abs(val_a - val_b) > 1e-9:
            if asc:
                return -1 if val_a < val_b else 1
            else:
                return -1 if val_a > val_b else 1

    # Tie-breaker: earliest timestamp wins
    ts_a = a.get("submitted_at", "9999")
    ts_b = b.get("submitted_at", "9999")
    if ts_a != ts_b:
        return -1 if ts_a < ts_b else 1
    return 0


def reduce_problem_scoreboard(
    entries: List[Dict[str, Any]], colspec: Optional[List[Dict[str, Any]]] = None
) -> List[Dict[str, Any]]:
    """
    Applies Autolab's scoreboard reduction:
    1. Filter out any entry where `score <= 0` or `scoreboard` is None (`zero_all_scoreboards` guard).
    2. Sort all valid entries by `colspec` + timestamp tie-breaker.
    3. Deduplicate by `user` (`DISTINCT ON (user)`), keeping only each user's best submission.
    4. Assign 1-based `rank`.
    """
    spec = colspec or DEFAULT_COLSPEC
    valid = [
        e for e in entries
        if e.get("scoreboard") is not None and float(e.get("score", 0.0)) > 0.0
    ]

    valid.sort(key=functools.cmp_to_key(lambda x, y: compare_autolab_entries(x, y, spec)))

    deduped: List[Dict[str, Any]] = []
    seen_users = set()
    for entry in valid:
        user = entry.get("user", "anonymous")
        if user in seen_users:
            continue
        seen_users.add(user)
        row = dict(entry)
        row["rank"] = len(deduped) + 1
        deduped.append(row)

    return deduped


def load_leaderboard(path: Path = LEADERBOARD_FILE) -> Dict[str, Any]:
    if not path.exists():
        return {
            "updated_at": datetime.now(timezone.utc).isoformat(),
            "problems": {},
            "global_rankings": [],
        }
    try:
        with open(path, "r", encoding="utf-8") as f:
            return json.load(f)
    except Exception:
        return {
            "updated_at": datetime.now(timezone.utc).isoformat(),
            "problems": {},
            "global_rankings": [],
        }


def recompute_global_rankings(problems_board: Dict[str, List[Dict[str, Any]]]) -> List[Dict[str, Any]]:
    """
    Aggregates per-problem Autolab boards into a global ranking across all catalog problems:
    - Primary sort: Total Problems Solved (DESC)
    - Secondary sort: Gold Medals / Rank #1 finishes (DESC)
    - Tertiary sort: Total Score (DESC)
    - Quaternary sort: Sum of best Cycles/Op across solved problems (ASC)
    """
    users: Dict[str, Dict[str, Any]] = {}

    for prob_id, rows in problems_board.items():
        for row in rows:
            u = row.get("user", "anonymous")
            if u not in users:
                users[u] = {
                    "user": u,
                    "solved_count": 0,
                    "first_place_count": 0,
                    "total_score": 0.0,
                    "total_cycles": 0.0,
                    "total_bytes": 0,
                    "solved_problems": [],
                }
            vec = row.get("scoreboard") or [100.0, 0.0, 0]
            users[u]["solved_count"] += 1
            if row.get("rank") == 1:
                users[u]["first_place_count"] += 1
            users[u]["total_score"] += float(vec[0]) if len(vec) > 0 else 100.0
            users[u]["total_cycles"] = round(
                users[u]["total_cycles"] + (float(vec[1]) if len(vec) > 1 else 0.0), 3
            )
            users[u]["total_bytes"] += int(vec[2]) if len(vec) > 2 else 0
            users[u]["solved_problems"].append(prob_id)

    ranked = sorted(
        users.values(),
        key=lambda x: (
            -x["solved_count"],
            -x["first_place_count"],
            -x["total_score"],
            x["total_cycles"],
            x["total_bytes"],
            x["user"],
        ),
    )
    for i, r in enumerate(ranked):
        r["rank"] = i + 1
    return ranked


def merge_scoreboard_entries(
    new_entries: List[Dict[str, Any]],
    leaderboard_path: Path = LEADERBOARD_FILE,
) -> Dict[str, Any]:
    """
    Merges one or more verified `scoreboard_entry` records into `leaderboard.json`,
    re-running Autolab's `colspec` reduction per problem and updating global standings.
    """
    catalog = ProblemCatalog()
    prob_map = {p["id"]: p for p in catalog.list_problems()}

    board = load_leaderboard(leaderboard_path)
    problems_board: Dict[str, List[Dict[str, Any]]] = board.get("problems", {})

    for entry in new_entries:
        prob_id = entry.get("problem_id")
        if not prob_id or entry.get("scoreboard") is None or float(entry.get("score", 0.0)) <= 0.0:
            continue
        colspec = prob_map.get(prob_id, {}).get("colspec", DEFAULT_COLSPEC)
        existing = problems_board.get(prob_id, [])
        existing.append(
            {
                "user": entry["user"],
                "problem_id": prob_id,
                "lang": entry.get("lang", "cpp"),
                "score": float(entry["score"]),
                "scoreboard": entry["scoreboard"],
                "submitted_at": entry.get("submitted_at") or datetime.now(timezone.utc).isoformat(),
                "commit_sha": entry.get("commit_sha", "local"),
            }
        )
        problems_board[prob_id] = reduce_problem_scoreboard(existing, colspec=colspec)

    board["updated_at"] = datetime.now(timezone.utc).isoformat()
    board["problems"] = problems_board
    board["global_rankings"] = recompute_global_rankings(problems_board)

    leaderboard_path.parent.mkdir(parents=True, exist_ok=True)
    with open(leaderboard_path, "w", encoding="utf-8") as f:
        json.dump(board, f, indent=2)

    return board


def verify_pr_isolation(changed_files: List[str], github_actor: str) -> Tuple[bool, str]:
    """
    Security boundary for GitHub Actions PR autograding:
    Ensures a contributor PR ONLY modifies files inside `submissions/<github_actor>/`
    and never touches `.github/workflows/`, `problems/`, or `tutor/`.
    """
    if not changed_files:
        return False, "No changed files detected in PR."

    allowed_prefix = f"submissions/{github_actor}/"
    for fpath in changed_files:
        norm = fpath.strip().lstrip("./")
        if not norm.startswith(allowed_prefix):
            return (
                False,
                f"Security violation: PR modified '{norm}', which is outside allowed prefix '{allowed_prefix}'.",
            )
        if ".." in norm.split("/"):
            return False, f"Security violation: path traversal in '{norm}'."

    return True, f"All {len(changed_files)} changed file(s) strictly isolated to {allowed_prefix}"


def export_static_site_data() -> Tuple[Path, Path]:
    """
    Exports `docs/data/catalog.json` (full curriculum + DAG + specs) and
    initializes `docs/data/leaderboard.json` if not present.
    """
    DOCS_DATA_DIR.mkdir(parents=True, exist_ok=True)
    catalog = ProblemCatalog()
    problems = catalog.list_problems()

    export_list = []
    for p in problems:
        prob_dir = Path(p["dir"])
        readme_path = prob_dir / "README.md"
        readme_text = p.get("spec") or (readme_path.read_text(encoding="utf-8") if readme_path.exists() else "")

        stub_text = ""
        harness_text = ""
        if (prob_dir / "sh" / "solution_stub.sh").exists():
            stub_text = (prob_dir / "sh" / "solution_stub.sh").read_text(encoding="utf-8")
            harness_path = prob_dir / "sh" / "harness.sh"
            harness_text = harness_path.read_text(encoding="utf-8") if harness_path.exists() else ""
        elif (prob_dir / "cpp" / "solution_stub.hpp").exists():
            stub_text = (prob_dir / "cpp" / "solution_stub.hpp").read_text(encoding="utf-8")
            harness_path = prob_dir / "cpp" / "harness.cpp"
            harness_text = harness_path.read_text(encoding="utf-8") if harness_path.exists() else ""

        export_list.append(
            {
                "id": p["id"],
                "title": p.get("title", p["id"]),
                "track": p.get("track", p.get("category", "Systems")),
                "category": p.get("category", "Systems"),
                "difficulty": p.get("difficulty", "Medium"),
                "depends_on": p.get("depends_on", []),
                "colspec": p.get("colspec", DEFAULT_COLSPEC),
                "languages": p.get("languages", ["cpp"]),
                "tags": p.get("tags", []),
                "spec": readme_text,
                "readme": readme_text,
                "stub": stub_text,
                "harness": harness_text,
            }
        )

    payload = {
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "total_problems": len(export_list),
        "problems": export_list,
    }
    with open(CATALOG_EXPORT_FILE, "w", encoding="utf-8") as f:
        json.dump(payload, f, indent=2)

    if not LEADERBOARD_FILE.exists():
        board = load_leaderboard(LEADERBOARD_FILE)
        with open(LEADERBOARD_FILE, "w", encoding="utf-8") as f:
            json.dump(board, f, indent=2)

    return CATALOG_EXPORT_FILE, LEADERBOARD_FILE
