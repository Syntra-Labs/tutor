import os
import shutil
import subprocess
from pathlib import Path
from typing import Dict, Any, Tuple

from tutor.state import get_config, USER_PROBLEMS, update_config

class SyncManager:
    """Handles pulling updated problem sets, test harnesses, and connecting to optional distributed nodes."""

    def __init__(self):
        self.config = get_config()
        self.remote_url = self.config.get("remote_url", "https://github.com/Syntra-Labs/tutor.git")
        self.distributed_node = self.config.get("distributed_node")

    def pull(self, custom_remote: str = None) -> Tuple[bool, str]:
        """Pulls the latest problem sets and test suites from the remote repository."""
        target_remote = custom_remote or self.remote_url
        if custom_remote:
            update_config({"remote_url": custom_remote})

        USER_PROBLEMS.parent.mkdir(parents=True, exist_ok=True)
        sync_repo_dir = USER_PROBLEMS.parent / "upstream_repo"

        # Check if already a git repo
        if (sync_repo_dir / ".git").exists():
            # git pull
            proc = subprocess.run(
                ["git", "pull", "--ff-only"],
                cwd=str(sync_repo_dir),
                capture_output=True,
                text=True,
            )
            if proc.returncode != 0:
                return False, f"Git pull failed:\n{proc.stderr}"
            msg = proc.stdout.strip()
        else:
            # git clone shallow
            if sync_repo_dir.exists():
                shutil.rmtree(sync_repo_dir)
            proc = subprocess.run(
                ["git", "clone", "--depth=1", target_remote, str(sync_repo_dir)],
                capture_output=True,
                text=True,
            )
            if proc.returncode != 0:
                return False, f"Git clone failed:\n{proc.stderr}"
            msg = "Cloned fresh problem repository."

        # Mirror problems folder into USER_PROBLEMS
        upstream_problems = sync_repo_dir / "problems"
        if upstream_problems.exists():
            if USER_PROBLEMS.exists():
                shutil.rmtree(USER_PROBLEMS)
            shutil.copytree(upstream_problems, USER_PROBLEMS)
            count = len([p for p in USER_PROBLEMS.iterdir() if p.is_dir()])
            return True, f"Successfully synchronized {count} problem sets from {target_remote}."
        else:
            return True, f"Synchronized repository, but no problems directory found."

    def connect_node(self, endpoint: str) -> Tuple[bool, str]:
        """Connects or registers this tutor client with an optional distributed cluster node."""
        update_config({"distributed_node": endpoint})
        return True, f"Configured distributed verification endpoint: {endpoint}\n(Solutions will benchmark locally and optionally submit telemetry to node)."
