#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 010 SWAR Balanced Ternary Multiplication Test Suite ===\n";

    PackedTrit64 zero{0ULL, 0ULL};
    PackedTrit64 ones{~0ULL, 0ULL};
    PackedTrit64 neg_ones{0ULL, ~0ULL};

    PackedTrit64 a{0x5555555555555555ULL, 0x2222222222222222ULL};
    PackedTrit64 b{0x3333333333333333ULL, 0xCCCCCCCCCCCCCCCCULL};
    PackedTrit64 c{0x0F0F0F0F0F0F0F0FULL, 0xF000F000F000F000ULL};
    assert(a.is_valid() && b.is_valid() && c.is_valid());

    // Test 1: Identity, zero absorption, and -1 involution
    PackedTrit64 a_id = trit_mul(a, ones);
    assert(a_id.pos == a.pos && a_id.neg == a.neg);

    PackedTrit64 a_zero = trit_mul(a, zero);
    assert(a_zero.pos == 0ULL && a_zero.neg == 0ULL);

    PackedTrit64 a_neg = trit_mul(a, neg_ones);
    assert(a_neg.pos == a.neg && a_neg.neg == a.pos);
    std::cout << "  • Test 1: Identity (+1), absorption (0), and negation (-1) ... [PASS]\n";

    // Test 2: Commutativity, associativity, and validity preservation
    PackedTrit64 ab = trit_mul(a, b);
    PackedTrit64 ba = trit_mul(b, a);
    assert(ab.is_valid());
    assert(ab.pos == ba.pos && ab.neg == ba.neg);

    PackedTrit64 abc1 = trit_mul(ab, c);
    PackedTrit64 abc2 = trit_mul(a, trit_mul(b, c));
    assert(abc1.is_valid());
    assert(abc1.pos == abc2.pos && abc1.neg == abc2.neg);
    std::cout << "  • Test 2: Commutativity, associativity & disjoint plane closure ... [PASS]\n";

    // Test 3: Square idempotence on support: a * a == { a.pos | a.neg, 0 }
    PackedTrit64 a_sq = trit_mul(a, a);
    assert(a_sq.pos == (a.pos | a.neg) && a_sq.neg == 0ULL);
    std::cout << "  • Test 3: Square projection onto active support mask ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 2000000;
    PackedTrit64 acc = a;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        acc = trit_mul(acc, b);
        acc.pos ^= (i & 1ULL);
        acc.neg &= ~acc.pos;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (acc.pos == 0xdeadbeefULL) std::cout << acc.neg;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 010 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
