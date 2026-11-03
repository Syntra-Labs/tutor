#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 028 Golden Integer Ring Z[phi] Suite ===\n";

    constexpr ZPhi one{1, 0};
    constexpr ZPhi phi{0, 1};

    // Test 1: Fundamental golden identity phi^2 == phi + 1
    assert((phi * phi) == (phi + one));
    assert((phi * phi) == (ZPhi{1, 1}));
    std::cout << "  • Test 1: Minimal polynomial phi^2 == phi + 1 == (1, 1) ... [PASS]\n";

    // Test 2: Reciprocal identity phi * (phi - 1) == 1
    ZPhi phi_inv{-1, 1};
    assert((phi * phi_inv) == one);
    std::cout << "  • Test 2: Reciprocal identity phi * (phi - 1) == (1, 0) ... [PASS]\n";

    // Test 3: Ring axioms (associativity & distributivity) & doubled icosian norm
    // Doubled icosian (phi, 1, phi^-1, 0) -> sum of squares = phi^2 + 1 + phi^-2 == 4
    ZPhi x{3, -2}, y{-5, 4}, z{7, 1};
    assert(((x * y) * z) == (x * (y * z)));
    assert((x * (y + z)) == ((x * y) + (x * z)));
    ZPhi sum_sq = (phi * phi) + (one * one) + (phi_inv * phi_inv);
    assert(sum_sq == (ZPhi{4, 0}));
    std::cout << "  • Test 3: Ring associativity, distributivity & icosian length^2 == 4 ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 1000000;
    ZPhi acc{1, 1};
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        acc = (acc * phi) - one;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    volatile int64_t sink = acc.a ^ acc.b;
    (void)sink;

    std::cout << "All 028 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
