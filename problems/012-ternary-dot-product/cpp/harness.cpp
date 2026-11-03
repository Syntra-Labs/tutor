#include <array>
#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <span>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 012 Multiplier-Free Ternary Dot Product Test Suite ===\n";

    std::array<PackedTrit64, 4> a{{
        {0x5555555555555555ULL, 0x2222222222222222ULL},
        {0x0F0F0F0F0F0F0F0FULL, 0xF000F000F000F000ULL},
        {0xFFFFFFFF00000000ULL, 0x00000000FFFFFFFFULL},
        {0x123456789ABCDEF0ULL, 0xEDCBA98765432100ULL & ~0x123456789ABCDEF0ULL}
    }};

    std::array<PackedTrit64, 4> b{{
        {0x3333333333333333ULL, 0xCCCCCCCCCCCCCCCCULL},
        {0x00FF00FF00FF00FFULL, 0xFF00FF0000000000ULL},
        {0xAAAA5555AAAA5555ULL, 0x5555AAAA5555AAAAULL},
        {0x0F0F0F0F0F0F0F0FULL, 0xF0F0F0F0F0F0F0F0ULL}
    }};

    // Test 1: Self dot product equals total non-zero trits (L2^2 == L1 on {-1,0,+1})
    int64_t expected_nnz_a = 0;
    for (const auto& w : a) {
        expected_nnz_a += std::popcount(w.pos) + std::popcount(w.neg);
    }
    assert(trit_dot(a, a) == expected_nnz_a);
    std::cout << "  • Test 1: Self inner product <a, a> == nnz(a) ... [PASS]\n";

    // Test 2: Exact agreement with scalar reference loop & commutativity
    int64_t scalar_ref = 0;
    for (size_t w = 0; w < 4; ++w) {
        for (int bit = 0; bit < 64; ++bit) {
            int va = static_cast<int>((a[w].pos >> bit) & 1ULL) - static_cast<int>((a[w].neg >> bit) & 1ULL);
            int vb = static_cast<int>((b[w].pos >> bit) & 1ULL) - static_cast<int>((b[w].neg >> bit) & 1ULL);
            scalar_ref += va * vb;
        }
    }
    int64_t dot_ab = trit_dot(a, b);
    int64_t dot_ba = trit_dot(b, a);
    assert(dot_ab == scalar_ref);
    assert(dot_ba == scalar_ref);
    std::cout << "  • Test 2: Exact agreement with scalar reference & symmetry ... [PASS]\n";

    // Test 3: Bilinear skew under negation <a, -b> == -<a, b>
    std::array<PackedTrit64, 4> neg_b{};
    for (size_t w = 0; w < 4; ++w) neg_b[w] = b[w].negate();
    assert(trit_dot(a, neg_b) == -dot_ab);
    assert(trit_dot(a, a) + trit_dot(a, std::array<PackedTrit64, 4>{
        a[0].negate(), a[1].negate(), a[2].negate(), a[3].negate()
    }) == 0);
    std::cout << "  • Test 3: Bilinear skew under negation <a, -b> == -<a, b> ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 500000;
    int64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        a[0].pos ^= (i & 1ULL);
        a[0].neg &= ~a[0].pos;
        sink += trit_dot(a, b);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (sink == 0xdeadbeefLL) std::cout << sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 012 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
