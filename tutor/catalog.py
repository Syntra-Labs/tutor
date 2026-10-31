import os
import json
from pathlib import Path
from typing import List, Dict, Any, Optional

from tutor.state import is_solved

REPO_ROOT = Path(__file__).resolve().parent.parent
BUILTIN_PROBLEMS = REPO_ROOT / "problems"
USER_PROBLEMS = Path.home() / ".local" / "share" / "tutor" / "problems"

class ProblemCatalog:
    """Discovers, indexes, and retrieves available problem sets."""

    def __init__(self):
        self.search_paths = [BUILTIN_PROBLEMS, USER_PROBLEMS]

    def list_problems(self) -> List[Dict[str, Any]]:
        problems = []
        seen_ids = set()

        for base_path in self.search_paths:
            if not base_path.exists():
                continue
            for prob_dir in sorted(base_path.iterdir()):
                if not prob_dir.is_dir():
                    continue
                meta_file = prob_dir / "problem.json"
                if not meta_file.exists():
                    continue
                try:
                    with open(meta_file, "r") as f:
                        meta = json.load(f)
                    pid = meta.get("id", prob_dir.name)
                    if pid in seen_ids:
                        continue
                    seen_ids.add(pid)
                    meta["dir"] = str(prob_dir)
                    meta["solved"] = is_solved(pid)
                    problems.append(meta)
                except Exception as e:
                    continue

        return sorted(problems, key=lambda x: x.get("id", ""))

    def get_problem(self, problem_id_or_num: str) -> Optional[Dict[str, Any]]:
        problems = self.list_problems()
        for p in problems:
            # Match exact ID (e.g. 001-lockfree-ringbuffer), number prefix (e.g. 1 or 001), or title substring
            pid = p.get("id", "")
            if pid == problem_id_or_num:
                return p
            if pid.startswith(f"{problem_id_or_num.zfill(3)}-"):
                return p
            if pid.split("-")[0].lstrip("0") == problem_id_or_num.lstrip("0"):
                return p
        return None
