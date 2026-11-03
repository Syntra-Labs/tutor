#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 032 256-Bit Unsigned Integer (U256) Suite ===\n";

    // Test 1: bits(), shl(), and shr_sticky() across the 128-bit limb boundary
    U256 zero{0, 0};
    U256 one{0, 1};
    assert(zero.bits() == 0);
    assert(one.bits() == 1);
    U256 shifted = one.shl(130);
    assert(shifted.hi == 4 && shifted.lo == 0);
    assert(shifted.bits() == 131);

    auto [r_exact, sticky_exact] = shifted.shr_sticky(130);
    assert(r_exact == one && !sticky_exact);

    U256 with_low = shifted.add(one);
    auto [r_inexact, sticky_inexact] = with_low.shr_sticky(130);
    assert(r_inexact == one && sticky_inexact);
    std::cout << "  • Test 1: bits(), shl(), and shr_sticky() across 128-bit boundary ... [PASS]\n";

    // Test 2: add() and sub() carry/borrow across lo -> hi
    U256 max_lo{0, ~static_cast<__uint128_t>(0)};
    U256 five{0, 5};
    U256 sum = max_lo.add(five);
    assert(sum.hi == 1 && sum.lo == 4);
    assert(sum.sub(five) == max_lo);
    std::cout << "  • Test 2: 256-bit add/sub carry and borrow propagation ... [PASS]\n";

    // Test 3: Exact 128x128 -> 256 multiplication (2^128 - 1) * (2^128 - 1) == 2^256 - 2*2^128 + 1
    __uint128_t all_ones = ~static_cast<__uint128_t>(0);
    U256 prod = U256::mul_u128(all_ones, all_ones);
    assert(prod.hi == (all_ones - 1));
    assert(prod.lo == 1);
    std::cout << "  • Test 3: Exact (2^128 - 1)^2 == (2^128 - 2)*2^128 + 1 ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 1000000;
    volatile uint64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        U256 p = U256::mul_u128(all_ones - i, all_ones - (i * 3));
        sink ^= static_cast<uint64_t>(p.hi ^ p.lo);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 032 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
