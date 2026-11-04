import os
import shutil
from pathlib import Path
from typing import Dict, Any, Tuple

from tutor.state import get_config

EXTENSIONS = {
    "cpp": "hpp",
    "rust": "rs",
    "python": "py",
    "c": "h",
    "go": "go",
    "sh": "sh",
}

class WorkspaceManager:
    """Scaffolds and manages local problem solutions and test environments."""

    def __init__(self):
        self.config = get_config()
        self.workspace_root = Path(self.config["workspace_dir"])

    def scaffold(self, problem: Dict[str, Any], lang: str = "cpp") -> Tuple[Path, Path]:
        """
        Scaffolds problem files in workspace_dir/problem_id.
        Returns (solution_file_path, problem_readme_path).
        """
        prob_id = problem["id"]
        prob_src_dir = Path(problem["dir"])
        target_dir = self.workspace_root / prob_id
        target_dir.mkdir(parents=True, exist_ok=True)

        # 1. Write Problem Specification
        spec_text = problem.get("spec")
        readme_src = prob_src_dir / "README.md"
        if spec_text:
            (target_dir / "spec.txt").write_text(spec_text, encoding="utf-8")
        elif readme_src.exists():
            readme_dst = target_dir / "spec.txt"
            shutil.copy2(readme_src, readme_dst)

        # 2. Check language implementation folder
        lang_dir = prob_src_dir / lang
        if not lang_dir.exists():
            # Fallback to first available language
            available = problem.get("languages", ["cpp"])
            lang = available[0]
            lang_dir = prob_src_dir / lang

        ext = EXTENSIONS.get(lang, "txt")
        solution_filename = f"solution.{ext}"
        solution_dst = target_dir / solution_filename

        # Copy starter stub if user has not written code yet
        stub_files = list(lang_dir.glob("solution_stub.*")) + list(lang_dir.glob("stub.*"))
        if not solution_dst.exists() and stub_files:
            shutil.copy2(stub_files[0], solution_dst)
        elif not solution_dst.exists():
            # Create empty file with header
            solution_dst.write_text(f"// Solution for {problem.get('title', prob_id)}\n")

        # 3. Copy test harness and build files
        for item in lang_dir.iterdir():
            if item.name.startswith("solution_stub") or item.name.startswith("stub"):
                continue
            dest_item = target_dir / item.name
            if item.is_file():
                shutil.copy2(item, dest_item)
            elif item.is_dir():
                if dest_item.exists():
                    shutil.rmtree(dest_item)
                shutil.copytree(item, dest_item)

        return solution_dst, readme_dst
