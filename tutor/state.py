import os
import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Dict, Any, Optional

STATE_DIR = Path.home() / ".local" / "share" / "tutor"
STATE_FILE = STATE_DIR / "progress.json"
CONFIG_FILE = STATE_DIR / "config.json"
USER_PROBLEMS = STATE_DIR / "problems"

DEFAULT_CONFIG = {
    "workspace_dir": str(Path.home() / "tutor-workspace"),
    "default_lang": "cpp",
    "remote_url": "https://github.com/Syntra-Labs/tutor.git",
    "distributed_node": None,  # Optional remote evaluator endpoint
}

def get_config() -> Dict[str, Any]:
    STATE_DIR.mkdir(parents=True, exist_ok=True)
    if not CONFIG_FILE.exists():
        with open(CONFIG_FILE, "w") as f:
            json.dump(DEFAULT_CONFIG, f, indent=2)
        return DEFAULT_CONFIG.copy()
    try:
        with open(CONFIG_FILE, "r") as f:
            data = json.load(f)
            merged = DEFAULT_CONFIG.copy()
            merged.update(data)
            return merged
    except Exception:
        return DEFAULT_CONFIG.copy()

def update_config(updates: Dict[str, Any]):
    config = get_config()
    config.update(updates)
    STATE_DIR.mkdir(parents=True, exist_ok=True)
    with open(CONFIG_FILE, "w") as f:
        json.dump(config, f, indent=2)

def get_progress() -> Dict[str, Any]:
    STATE_DIR.mkdir(parents=True, exist_ok=True)
    if not STATE_FILE.exists():
        return {}
    try:
        with open(STATE_FILE, "r") as f:
            return json.load(f)
    except Exception:
        return {}

def mark_solved(
    problem_id: str,
    lang: str,
    runtime_ms: float = 0.0,
    cycles_per_op: float = 0.0,
    code_bytes: int = 0,
):
    STATE_DIR.mkdir(parents=True, exist_ok=True)
    progress = get_progress()
    if problem_id not in progress:
        progress[problem_id] = {}

    prev = progress[problem_id].get(lang, {})
    best_cycles = cycles_per_op
    if prev.get("cycles_per_op", 0.0) > 0.0 and cycles_per_op > 0.0:
        best_cycles = min(prev["cycles_per_op"], cycles_per_op)

    progress[problem_id][lang] = {
        "status": "solved",
        "runtime_ms": round(runtime_ms, 3),
        "cycles_per_op": round(best_cycles, 3),
        "code_bytes": code_bytes,
        "solved_at": datetime.now(timezone.utc).isoformat(),
    }
    with open(STATE_FILE, "w") as f:
        json.dump(progress, f, indent=2)

def is_solved(problem_id: str, lang: Optional[str] = None) -> bool:
    progress = get_progress()
    p = progress.get(problem_id, {})
    if lang:
        return p.get(lang, {}).get("status") == "solved"
    return any(details.get("status") == "solved" for details in p.values())
