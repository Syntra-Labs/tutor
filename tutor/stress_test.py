"""Comprehensive stress tests for compute-intensive workloads, edge-cases, and concurrency."""
import concurrent.futures
import os
import sys
import tempfile
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tutor.catalog import ProblemCatalog
from tutor.runner import TestRunner


def run_all_stress_tests():
    cat = ProblemCatalog()
    runner = TestRunner()

    print("=" * 68)
    print("  SYNTRA TUTOR: COMPUTE INTENSITY & EDGE-CASE STRESS TEST SUITE")
    print("=" * 68)

    # 1. 2,000,000 Iterations CA Rule 30
    prob_006 = cat.get_problem("006-ca30-chaos-step")
    sol_006 = """#pragma once
#include <bit>
#include <cstdint>
inline uint64_t step_rule_30(uint64_t state) {
    uint64_t l = std::rotl(state, 1);
    uint64_t r = std::rotr(state, 1);
    return l ^ (state | r);
}
"""
    with tempfile.TemporaryDirectory() as td:
        (Path(td) / "solution.hpp").write_text(sol_006)
        t0 = time.perf_counter()
        ok, log, rt, met = runner.run_test(prob_006, lang="cpp", submission_dir=Path(td))
        dt = (time.perf_counter() - t0) * 1000
        assert ok, f"006 failed: {log}"
        c_op = met.get("cycles_per_op")
        print(f"✓ TEST 1 [Heavy Compute - 2M iterations CA Rule 30]: PASSED in {rt:.2f}ms (wall: {dt:.2f}ms, cycles_per_op: {c_op})")

    # 2. 2,000,000 Iterations SWAR Balanced Ternary Multiplication
    prob_010 = cat.get_problem("010-swar-trit-multiply")
    sol_010 = """#pragma once
#include <cstdint>
struct PackedTrit64 {
    uint64_t pos = 0;
    uint64_t neg = 0;
    constexpr bool is_valid() const { return (pos & neg) == 0; }
};
constexpr PackedTrit64 trit_mul(PackedTrit64 a, PackedTrit64 b) {
    PackedTrit64 out;
    out.pos = (a.pos & b.pos) | (a.neg & b.neg);
    out.neg = (a.pos & b.neg) | (a.neg & b.pos);
    return out;
}
"""
    with tempfile.TemporaryDirectory() as td:
        (Path(td) / "solution.hpp").write_text(sol_010)
        t0 = time.perf_counter()
        ok, log, rt, met = runner.run_test(prob_010, lang="cpp", submission_dir=Path(td))
        dt = (time.perf_counter() - t0) * 1000
        assert ok, f"010 failed: {log}"
        c_op = met.get("cycles_per_op")
        print(f"✓ TEST 2 [Heavy Compute - 2M trit ops SWAR]: PASSED in {rt:.2f}ms (wall: {dt:.2f}ms, cycles_per_op: {c_op})")

    # 3. Stack Overflow (SIGSEGV)
    prob_000 = cat.get_problem("000a-hello-world")
    sol_stack = """#pragma once
#include <string>
#include <string_view>
int rec(int n) { volatile char b[8192]; b[0] = n; return rec(n + 1) + b[0]; }
inline std::string hello_world() { rec(1); return "Hello, World!"; }
inline std::string greet(std::string_view n) { return "Hello, World!"; }
"""
    with tempfile.TemporaryDirectory() as td:
        (Path(td) / "solution.hpp").write_text(sol_stack)
        ok, log, rt, met = runner.run_test(prob_000, lang="cpp", submission_dir=Path(td))
        assert not ok, "Stack overflow should fail!"
        assert "SIGSEGV" in log or "Segmentation Fault" in log, f"Expected SIGSEGV, got: {log}"
        last_line = log.strip().splitlines()[-1]
        print(f"✓ TEST 3 [Stack Overflow SIGSEGV caught cleanly]: PASSED -> {last_line}")

    # 4. Division by Zero (SIGFPE)
    sol_fpe = """#pragma once
#include <string>
#include <string_view>
inline std::string hello_world() {
    volatile int z = 0;
    volatile int x = 100 / z;
    (void)x;
    return "Hello, World!";
}
inline std::string greet(std::string_view n) { return "Hello, World!"; }
"""
    with tempfile.TemporaryDirectory() as td:
        (Path(td) / "solution.hpp").write_text(sol_fpe)
        ok, log, rt, met = runner.run_test(prob_000, lang="cpp", submission_dir=Path(td))
        assert not ok, "Division by zero should fail!"
        assert "SIGFPE" in log or "Floating Point Exception" in log, f"Expected SIGFPE, got: {log}"
        last_line = log.strip().splitlines()[-1]
        print(f"✓ TEST 4 [Division by Zero SIGFPE caught cleanly]: PASSED -> {last_line}")

    # 5. Heap Exhaustion (std::bad_alloc / SIGABRT)
    sol_mle = """#pragma once
#include <string>
#include <string_view>
#include <new>
inline std::string hello_world() {
    throw std::bad_alloc();
    return "Hello, World!";
}
inline std::string greet(std::string_view n) { return "Hello, World!"; }
"""
    with tempfile.TemporaryDirectory() as td:
        (Path(td) / "solution.hpp").write_text(sol_mle)
        ok, log, rt, met = runner.run_test(prob_000, lang="cpp", submission_dir=Path(td))
        assert not ok, "Heap exhaustion should fail!"
        assert "SIGABRT" in log or "Aborted" in log or "bad_alloc" in log, f"Expected SIGABRT, got: {log}"
        last_line = log.strip().splitlines()[-1]
        print(f"✓ TEST 5 [Heap Exhaustion std::bad_alloc caught cleanly]: PASSED -> {last_line}")

    # 6. Concurrency / Parallel Load (5 simultaneous compilations + benchmarks)
    def run_concurrent_worker(idx):
        with tempfile.TemporaryDirectory() as td:
            (Path(td) / "solution.hpp").write_text(sol_006)
            ok, log, rt, met = runner.run_test(prob_006, lang="cpp", submission_dir=Path(td))
            return ok, rt

    t0 = time.perf_counter()
    with concurrent.futures.ThreadPoolExecutor(max_workers=5) as ex:
        futs = [ex.submit(run_concurrent_worker, i) for i in range(5)]
        res = [f.result() for f in futs]
    conc_time = (time.perf_counter() - t0) * 1000
    assert all(r[0] for r in res), "Concurrent workers failed!"
    print(f"✓ TEST 6 [Concurrency: 5 Parallel Heavy Benchmarks]: ALL 5 PASSED in {conc_time:.2f}ms total")

    # 7. Shell Harness Compute / Git Object Plumbing
    prob_076 = cat.get_problem("076-git-object-plumbing")
    sol_076 = """create_plumbing_commit() {
    REPO_DIR="$1"
    MESSAGE="$2"
    mkdir -p "$REPO_DIR"
    cd "$REPO_DIR"
    git init -b main 2>/dev/null || git init
    echo "Hello Syntra Labs" > README.md
    mkdir -p src
    echo 'echo "Syntra Systems"' > src/main.sh
    H1=$(git hash-object -w README.md)
    H2=$(git hash-object -w src/main.sh)
    git update-index --add --cacheinfo 100644 "$H1" README.md
    git update-index --add --cacheinfo 100644 "$H2" src/main.sh
    TREE=$(git write-tree)
    COMMIT=$(git commit-tree "$TREE" -m "$MESSAGE")
    git update-ref refs/heads/main "$COMMIT"
    cd - >/dev/null
}
"""
    with tempfile.TemporaryDirectory() as td:
        (Path(td) / "solution.sh").write_text(sol_076)
        (Path(td) / "solution.sh").chmod(0o755)
        ok, log, rt, met = runner.run_test(prob_076, lang="sh", submission_dir=Path(td))
        assert ok, f"076 failed: {log}"
        tc_name = met["testcases"][0]["name"]
        print(f"✓ TEST 7 [Shell Harness Native Execution]: PASSED in {rt:.2f}ms ({tc_name})")

    print("=" * 68)
    print("  ✓ ALL 7 STRESS TESTS SUCCEEDED WITHOUT LEAKS OR STALLS!")
    print("=" * 68)


if __name__ == "__main__":
    run_all_stress_tests()
