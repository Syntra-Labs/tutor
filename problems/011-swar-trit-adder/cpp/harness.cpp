#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

static int lane_trit(PackedTrit64 x, int lane) {
    int p = static_cast<int>((x.pos >> lane) & 1ULL);
    int n = static_cast<int>((x.neg >> lane) & 1ULL);
    return p - n;
}

int main() {
    std::cout << "=== Running 011 SWAR Balanced Ternary Full-Adder Test Suite ===\n";

    // Test 1: Exhaustive 27-state truth table packed into lanes 0..26
    PackedTrit64 a{}, b{}, c{};
    const int vals[3] = {-1, 0, 1};
    int lane = 0;
    for (int va : vals) {
        for (int vb : vals) {
            for (int vc : vals) {
                if (va == 1) a.pos |= (1ULL << lane);
                if (va == -1) a.neg |= (1ULL << lane);
                if (vb == 1) b.pos |= (1ULL << lane);
                if (vb == -1) b.neg |= (1ULL << lane);
                if (vc == 1) c.pos |= (1ULL << lane);
                if (vc == -1) c.neg |= (1ULL << lane);
                ++lane;
            }
        }
    }

    TritAddResult res = trit_full_add(a, b, c);
    assert(res.sum.is_valid());
    assert(res.carry.is_valid());

    for (int i = 0; i < 27; ++i) {
        int lhs = lane_trit(a, i) + lane_trit(b, i) + lane_trit(c, i);
        int rhs = 3 * lane_trit(res.carry, i) + lane_trit(res.sum, i);
        assert(lhs == rhs);
    }
    std::cout << "  • Test 1: Exhaustive 27-state truth table (a + b + c == 3*carry + sum) ... [PASS]\n";

    // Test 2: Symmetry / commutativity of all 3 operands
    TritAddResult r_bca = trit_full_add(b, c, a);
    TritAddResult r_cab = trit_full_add(c, a, b);
    assert(res.sum.pos == r_bca.sum.pos && res.sum.neg == r_bca.sum.neg);
    assert(res.carry.pos == r_bca.carry.pos && res.carry.neg == r_bca.carry.neg);
    assert(res.sum.pos == r_cab.sum.pos && res.sum.neg == r_cab.sum.neg);
    assert(res.carry.pos == r_cab.carry.pos && res.carry.neg == r_cab.carry.neg);
    std::cout << "  • Test 2: Full 3-input permutation symmetry ... [PASS]\n";

    // Test 3: Negation equivariance: full_add(-a, -b, -c) == (-sum, -carry)
    TritAddResult r_neg = trit_full_add(a.negate(), b.negate(), c.negate());
    assert(r_neg.sum.pos == res.sum.neg && r_neg.sum.neg == res.sum.pos);
    assert(r_neg.carry.pos == res.carry.neg && r_neg.carry.neg == res.carry.pos);
    std::cout << "  • Test 3: Negation equivariance ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 1000000;
    PackedTrit64 acc = a;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto step = trit_full_add(acc, b, c);
        acc = step.sum;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (acc.pos == 0xdeadbeefULL) std::cout << acc.neg;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 011 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
