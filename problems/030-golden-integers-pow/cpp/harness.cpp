#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 030 Z[phi] Binary Exponentiation & Fibonacci Identity Suite ===\n";

    // Test 1: Positive powers phi^n == (F_{n-1}, F_n)
    assert(golden_pow(0) == (ZPhi{1, 0}));
    assert(golden_pow(1) == (ZPhi{0, 1}));
    assert(golden_pow(2) == (ZPhi{1, 1}));
    assert(golden_pow(3) == (ZPhi{1, 2}));
    assert(golden_pow(10) == (ZPhi{34, 55}));
    assert(golden_pow(20) == (ZPhi{4181, 6765}));
    std::cout << "  • Test 1: Fibonacci identity phi^n == (F_{n-1}, F_n) ... [PASS]\n";

    // Test 2: Negative powers phi^{-n} * phi^n == (1, 0)
    for (int64_t n = -15; n <= 15; ++n) {
        ZPhi p_pos = golden_pow(n);
        ZPhi p_neg = golden_pow(-n);
        assert((p_pos * p_neg) == (ZPhi{1, 0}));
    }
    std::cout << "  • Test 2: Two-sided group law phi^n * phi^{-n} == 1 ... [PASS]\n";

    // Test 3: Cassini's Identity N(phi^n) == (-1)^n
    for (int64_t n = -20; n <= 20; ++n) {
        int64_t expected_norm = (n % 2 == 0) ? 1 : -1;
        assert(golden_pow(n).norm() == expected_norm);
    }
    std::cout << "  • Test 3: Cassini's identity N(phi^n) == (-1)^n ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 500000;
    volatile int64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        ZPhi p = golden_pow(static_cast<int64_t>((i & 63) - 32));
        sink ^= p.b;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 030 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
