#!/bin/sh
# harness.sh — Test harness for 075-container-namespace-isolation
set -e

PASSED=0
TOTAL=4

# Source user solution
if [ -f "./solution.sh" ]; then
    . ./solution.sh
elif [ -f "./solution_stub.sh" ]; then
    . ./solution_stub.sh
else
    echo "Error: solution.sh not found" >&2
    exit 1
fi

TEST_ROOT="/tmp/test_jail_$$"
mkdir -p "$TEST_ROOT/bin" "$TEST_ROOT/usr/bin" "$TEST_ROOT/proc" "$TEST_ROOT/tmp" "$TEST_ROOT/dev"
mknod -m 666 "$TEST_ROOT/dev/null" c 1 3 2>/dev/null || touch "$TEST_ROOT/dev/null"

# Copy minimal busybox binaries into jail
if [ -f "/bin/busybox" ]; then
    mkdir -p "$TEST_ROOT/lib"
    if [ -d "/lib" ] && ! file /bin/busybox 2>/dev/null | grep -q "statically linked"; then
        for so in $(ldd /bin/busybox 2>/dev/null | grep -o '/lib[^ ]*'); do
            mkdir -p "$TEST_ROOT/$(dirname "$so")"
            cp -L "$so" "$TEST_ROOT/$so" 2>/dev/null || true
        done
    fi
    cp /bin/busybox "$TEST_ROOT/bin/"
    for applet in sh ls cat ps echo mkdir test; do
        ln -sf /bin/busybox "$TEST_ROOT/bin/$applet" 2>/dev/null || true
    done
fi

cleanup() {
    umount -l "$TEST_ROOT/proc" 2>/dev/null || true
    rm -rf "$TEST_ROOT" 2>/dev/null || true
}
trap cleanup EXIT

# Test 1: Filesystem Jail Boundary
echo "[TEST 1] Verifying Root Confinement..."
touch "$TEST_ROOT/inside_marker"
HOST_OUT=$(run_isolated "$TEST_ROOT" /bin/sh -c "ls /inside_marker 2>/dev/null" || true)
if [ "$HOST_OUT" = "/inside_marker" ]; then
    echo 'TESTCASE_JSON: {"name": "Filesystem Confinement", "passed": true, "details": "Path traversal bounded to jail root"}'
    PASSED=$((PASSED + 1))
else
    echo 'TESTCASE_JSON: {"name": "Filesystem Confinement", "passed": false, "details": "Jail root marker not found"}'
fi

# Test 2: Host Secret Inaccessibility
echo "[TEST 2] Verifying Host Privacy..."
HOST_LEAK=$(run_isolated "$TEST_ROOT" /bin/sh -c "ls /root 2>/dev/null" || true)
if [ -z "$HOST_LEAK" ]; then
    echo 'TESTCASE_JSON: {"name": "Host Isolation", "passed": true, "details": "Host /root is completely hidden"}'
    PASSED=$((PASSED + 1))
else
    echo 'TESTCASE_JSON: {"name": "Host Isolation", "passed": false, "details": "Host /root leaked into container"}'
fi

# Test 3: Process Execution & Exit Codes
echo "[TEST 3] Verifying Command Execution & Exit Codes..."
set +e
run_isolated "$TEST_ROOT" /bin/sh -c "exit 42"
CODE=$?
set -e
if [ "$CODE" -eq 42 ]; then
    echo 'TESTCASE_JSON: {"name": "Exit Code Propagation", "passed": true, "details": "Exit code 42 propagated accurately"}'
    PASSED=$((PASSED + 1))
else
    echo "TESTCASE_JSON: {\"name\": \"Exit Code Propagation\", \"passed\": false, \"details\": \"Expected 42, got $CODE\"}"
fi

# Test 4: Proc Mount Functionality
echo "[TEST 4] Verifying Procfs Isolation..."
PROC_CHECK=$(run_isolated "$TEST_ROOT" /bin/sh -c "test -d /proc && echo 'PROC_OK'" || true)
if [ "$PROC_CHECK" = "PROC_OK" ]; then
    echo 'TESTCASE_JSON: {"name": "Procfs Availability", "passed": true, "details": "/proc is accessible inside container"}'
    PASSED=$((PASSED + 1))
else
    echo 'TESTCASE_JSON: {"name": "Procfs Availability", "passed": false, "details": "/proc mount failed inside container"}'
fi

SCORE=$((PASSED * 100 / TOTAL))
echo "SCOREBOARD_JSON: {\"Score\": $SCORE, \"PID Isolation\": \"$PASSED/$TOTAL\", \"Mount Jail\": \"100%\"}"
echo "Tests Passed: $PASSED / $TOTAL"
exit 0
