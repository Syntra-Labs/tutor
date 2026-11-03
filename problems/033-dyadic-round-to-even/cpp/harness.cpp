#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 033 Bounded-Precision Dyadic Dy<P> Rounding Suite ===\n";

    // Test 1: Canonical representation strips trailing zeros (12 = 3 * 2^2, 0 -> {0, 0})
    auto z = Dy<8>::from_i64(0);
    assert(z.m == 0 && z.e == 0);
    auto d12 = Dy<8>::from_i64(12);
    assert(d12.m == 3 && d12.e == 2);
    auto dn24 = Dy<8>::from_i64(-24);
    assert(dn24.m == -3 && dn24.e == 3);
    std::cout << "  • Test 1: Canonical odd-mantissa reduction ... [PASS]\n";

    // Test 2: Exact ties-to-even vs sticky-bit rounding at P = 8 (max mantissa < 256)
    // 9-bit number 257 = (128 * 2 + 1): kept m = 128 (even), round_bit = 1, low = 0:
    // If sticky=false -> tie to even -> m = 128 = 1 * 2^7, e = 1 + 7 = 8
    auto t257_even = Dy<8>::round(false, U256{0, 257}, 0, false);
    assert(t257_even.m == 1 && t257_even.e == 8);
    // If sticky=true -> strictly above midpoint -> m = 129, e = 1
    auto t257_sticky = Dy<8>::round(false, U256{0, 257}, 0, true);
    assert(t257_sticky.m == 129 && t257_sticky.e == 1);
    // 259 = (129 * 2 + 1): kept m = 129 (odd), round_bit = 1, sticky=false -> tie to even rounds up to 130 = 65 * 2^1 -> m = 65, e = 2
    auto t259_odd = Dy<8>::round(false, U256{0, 259}, 0, false);
    assert(t259_odd.m == 65 && t259_odd.e == 2);
    std::cout << "  • Test 2: Ties-to-even & sticky remainder rounding ... [PASS]\n";

    // Test 3: Mantissa overflow across 2^P boundary (511 = 255*2 + 1 -> rounds up to 256 -> shifts to 1 * 2^9)
    auto t511 = Dy<8>::from_i64(511);
    assert(t511.m == 1 && t511.e == 9);
    // And exact multiplication in Dy<120>
    auto a = Dy<120>::from_i64(1000000007LL);
    auto b = Dy<120>::from_i64(-1000000009LL);
    auto p = a.mul(b);
    assert(p.m == static_cast<__int128>(1000000007LL) * static_cast<__int128>(-1000000009LL));
    assert(p.e == 0);
    std::cout << "  • Test 3: Carry-out re-normalization & Dy<120> multiplication ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 1000000;
    Dy<120> cur = Dy<120>::from_i64(3);
    Dy<120> step = Dy<120>::from_i64(5);
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        cur = cur.mul(step);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    volatile int32_t sink = cur.e;
    (void)sink;

    std::cout << "All 033 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
