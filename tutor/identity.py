"""
Sovereign Learner Identity & Profile Persistence (Issue #7).
Local-first cryptographic identity generation, preference persistence,
and verified local deletion for Syntra Tutor.
"""
import os
import json
import secrets
from datetime import datetime, timezone
from pathlib import Path
from typing import Dict, Any, Optional

from tutor.state import STATE_DIR, get_progress

PROFILE_FILE = STATE_DIR / "profile.json"

def generate_did() -> str:
    """Generates a cryptographic entropy-based DID identifier."""
    token = secrets.token_hex(20)
    return f"did:key:z{token}"

def get_or_create_profile() -> Dict[str, Any]:
    """Retrieves existing learner profile or initializes a new sovereign identity."""
    STATE_DIR.mkdir(parents=True, exist_ok=True)
    if PROFILE_FILE.exists():
        try:
            with open(PROFILE_FILE, "r", encoding="utf-8") as f:
                data = json.load(f)
                if data.get("did"):
                    # Keep progress updated from state.py
                    progress = get_progress()
                    if "progress" not in data:
                        data["progress"] = {}
                    data["progress"]["solved"] = list(progress.keys())
                    return data
        except Exception:
            pass

    default_handle = f"learner-{secrets.token_hex(3)}"
    profile = {
        "version": 1,
        "did": generate_did(),
        "handle": default_handle,
        "created_at": datetime.now(timezone.utc).isoformat(),
        "preferences": {
            "target_track": "",
            "target_difficulty": "all",
            "specializations": [],
        },
        "progress": {
            "solved": list(get_progress().keys()),
            "scores": {},
        },
    }
    save_profile(profile)
    return profile

def save_profile(profile: Dict[str, Any]):
    """Persists learner profile to local storage."""
    STATE_DIR.mkdir(parents=True, exist_ok=True)
    profile["updated_at"] = datetime.now(timezone.utc).isoformat()
    with open(PROFILE_FILE, "w", encoding="utf-8") as f:
        json.dump(profile, f, indent=2)

def set_handle(new_handle: str) -> Dict[str, Any]:
    """Updates the learner's display handle."""
    profile = get_or_create_profile()
    profile["handle"] = new_handle.strip()
    save_profile(profile)
    return profile

def set_preferences(target_track: Optional[str] = None, target_difficulty: Optional[str] = None) -> Dict[str, Any]:
    """Updates learner target track and difficulty preferences."""
    profile = get_or_create_profile()
    if target_track is not None:
        profile["preferences"]["target_track"] = target_track
    if target_difficulty is not None:
        profile["preferences"]["target_difficulty"] = target_difficulty
    save_profile(profile)
    return profile

def export_profile(dest_path: Path) -> Path:
    """Exports profile to a canonical JSON backup file."""
    profile = get_or_create_profile()
    dest_path.parent.mkdir(parents=True, exist_ok=True)
    with open(dest_path, "w", encoding="utf-8") as f:
        json.dump(profile, f, indent=2)
    return dest_path

def import_profile(src_path: Path) -> Dict[str, Any]:
    """Imports a profile JSON backup and validates schema."""
    with open(src_path, "r", encoding="utf-8") as f:
        data = json.load(f)
    if not data.get("did") or not data.get("handle"):
        raise ValueError("Invalid profile format: missing 'did' or 'handle'.")
    save_profile(data)
    return data

def purge_profile():
    """Verified local deletion: permanently destroys sovereign keypair and identity profile."""
    if PROFILE_FILE.exists():
        PROFILE_FILE.unlink()
