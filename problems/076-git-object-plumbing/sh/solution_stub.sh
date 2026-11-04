#!/bin/sh
# solution_stub.sh — Git Internals: Object Database Plumbing
# Implement create_plumbing_commit() using only low-level Git plumbing commands.
# (Do NOT invoke 'git add' or 'git commit')

create_plumbing_commit() {
    REPO_DIR="$1"
    MESSAGE="$2"

    mkdir -p "$REPO_DIR"
    cd "$REPO_DIR"
    git init -b main 2>/dev/null || git init

    # Create target payload files
    echo "Hello Syntra Labs" > README.md
    mkdir -p src
    echo 'echo "Syntra Systems"' > src/main.sh

    # TODO:
    # 1. Write blobs to object store: git hash-object -w <file>
    # 2. Stage to index: git update-index --add --cacheinfo 100644 <hash> <path>
    # 3. Write tree object: git write-tree
    # 4. Create commit object: git commit-tree <tree> -m "$MESSAGE"
    # 5. Update branch ref: git update-ref refs/heads/main <commit>

    cd - >/dev/null
}
