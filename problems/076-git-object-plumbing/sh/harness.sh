#!/bin/sh
# harness.sh — Test harness for 076-git-object-plumbing
set -e

PASSED=0
TOTAL=4

if [ -f "./solution.sh" ]; then
    . ./solution.sh
elif [ -f "./solution_stub.sh" ]; then
    . ./solution_stub.sh
else
    echo "Error: solution.sh not found" >&2
    exit 1
fi

TEST_REPO="/tmp/test_git_repo_$$"
cleanup() {
    rm -rf "$TEST_REPO" 2>/dev/null || true
}
trap cleanup EXIT

# Wrap git to intercept prohibited porcelain commands
intercept_dir="/tmp/git_intercept_$$"
mkdir -p "$intercept_dir"
REAL_GIT=$(which git 2>/dev/null || echo "/usr/bin/git")

cat << 'EOF' > "$intercept_dir/git"
#!/bin/sh
if [ "$1" = "add" ] || [ "$1" = "commit" ]; then
    echo "ERROR_PORCELAIN_FORBIDDEN: $1 is prohibited! Use plumbing commands only." >&2
    exit 99
fi
REAL_GIT_BIN=$(cat /tmp/real_git_path)
exec "$REAL_GIT_BIN" "$@"
EOF
echo "$REAL_GIT" > /tmp/real_git_path
chmod +x "$intercept_dir/git"

# Run user solution with intercepted PATH
echo "[PLUMBING HARNESS] Running create_plumbing_commit..."
OLD_PATH="$PATH"
export PATH="$intercept_dir:$PATH"

set +e
create_plumbing_commit "$TEST_REPO" "feat: manual plumbing commit"
USER_EXIT=$?
export PATH="$OLD_PATH"
set -e

if [ "$USER_EXIT" -ne 0 ]; then
    echo "TESTCASE_JSON: {\"name\": \"Plumbing Purity\", \"passed\": false, \"details\": \"Execution failed or invoked porcelain command (exit $USER_EXIT)\"}"
else
    echo 'TESTCASE_JSON: {"name": "Plumbing Purity", "passed": true, "details": "Strictly avoided git add and git commit"}'
    PASSED=$((PASSED + 1))
fi

cd "$TEST_REPO"

# Test 2: Repository Integrity via git fsck
echo "[TEST 2] Verifying Git Object Database Integrity..."
set +e
"$REAL_GIT" fsck --full >/dev/null 2>&1
FSCK_RES=$?
set -e
if [ "$FSCK_RES" -eq 0 ]; then
    echo 'TESTCASE_JSON: {"name": "Database Integrity", "passed": true, "details": "git fsck verified all SHA-1 tree/commit objects"}'
    PASSED=$((PASSED + 1))
else
    echo 'TESTCASE_JSON: {"name": "Database Integrity", "passed": false, "details": "Corrupted or broken git object graph"}'
fi

# Test 3: Commit Object & Tree Structure
echo "[TEST 3] Verifying Commit Object & Log..."
HEAD_COMMIT=$("$REAL_GIT" rev-parse --verify refs/heads/main 2>/dev/null || true)
if [ -n "$HEAD_COMMIT" ]; then
    COMMIT_MSG=$("$REAL_GIT" log -1 --pretty=%B "$HEAD_COMMIT" 2>/dev/null | tr -d '\r\n')
    if [ "$COMMIT_MSG" = "feat: manual plumbing commit" ]; then
        echo 'TESTCASE_JSON: {"name": "Commit Graph", "passed": true, "details": "refs/heads/main correctly points to valid commit object"}'
        PASSED=$((PASSED + 1))
    else
        echo "TESTCASE_JSON: {\"name\": \"Commit Graph\", \"passed\": false, \"details\": \"Commit message mismatch: '$COMMIT_MSG'\"}"
    fi
else
    echo 'TESTCASE_JSON: {"name": "Commit Graph", "passed": false, "details": "refs/heads/main ref pointer missing"}'
fi

# Test 4: File Content Checkout
echo "[TEST 4] Verifying File Blobs..."
README_CONTENT=$("$REAL_GIT" show main:README.md 2>/dev/null | tr -d '\r\n' || true)
SRC_CONTENT=$("$REAL_GIT" show main:src/main.sh 2>/dev/null | tr -d '\r\n' || true)
if [ "$README_CONTENT" = "Hello Syntra Labs" ] && [ "$SRC_CONTENT" = 'echo "Syntra Systems"' ]; then
    echo 'TESTCASE_JSON: {"name": "Blob Reconstruction", "passed": true, "details": "All files reconstructed cleanly from tree blobs"}'
    PASSED=$((PASSED + 1))
else
    echo 'TESTCASE_JSON: {"name": "Blob Reconstruction", "passed": false, "details": "Tree blob mismatch or missing files"}'
fi

SCORE=$((PASSED * 100 / TOTAL))
echo "SCOREBOARD_JSON: {\"Score\": $SCORE, \"Plumbing Pass\": \"$PASSED/$TOTAL\", \"Tree Integrity\": \"100%\"}"
echo "Tests Passed: $PASSED / $TOTAL"
exit 0
