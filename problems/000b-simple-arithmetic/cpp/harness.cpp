#include "solution.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

int main() {
    std::cout << "=== Running 000b Simple Arithmetic Test Suite ===\n";

    std::cout << "  - Test 1: add_i64 ... ";
    assert(add_i64(2, 3) == 5);
    assert(add_i64(-10, 10) == 0);
    assert(add_i64(1000000000LL, 2000000000LL) == 3000000000LL);
    std::cout << "[PASS]\n";

    std::cout << "  - Test 2: eval_quadratic (a*x^2 + b*x + c) ... ";
    assert(eval_quadratic(1, 0, 0, 5) == 25);
    assert(eval_quadratic(2, 3, -5, 4) == 2 * 16 + 12 - 5);
    assert(eval_quadratic(-1, 2, 1, -3) == -9 - 6 + 1);
    std::cout << "[PASS]\n";

    std::cout << "  - Test 3: sum_first_n (n*(n+1)/2) ... ";
    assert(sum_first_n(-5) == 0);
    assert(sum_first_n(0) == 0);
    assert(sum_first_n(10) == 55);
    assert(sum_first_n(100) == 5050);
    assert(sum_first_n(100000) == 5000050000LL);
    std::cout << "[PASS]\n";

    constexpr int ITERS = 1000000;
    auto t0 = std::chrono::high_resolution_clock::now();
    volatile int64_t sink = 0;
    for (int i = 0; i < ITERS; ++i) {
        sink = sink + add_i64(i, 1) + eval_quadratic(1, 2, 3, i & 15) + sum_first_n(i & 63);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;

    double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
    double cycles_per_op = (ns * 3.2) / (ITERS * 3.0);
    std::cout << "SCOREBOARD_JSON:{\"cycles_per_op\":" << cycles_per_op << "}\n";
    std::cout << "All 000b tests passed!\n";
    return 0;
}
