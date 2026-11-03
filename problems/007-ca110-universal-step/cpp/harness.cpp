#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 007 CA Rule 110 Step Test Suite ===\n";

    // Test 1: Quiescent, saturated, and single-cell truth table verification
    assert(step_rule_110(0ULL) == 0ULL);
    assert(step_rule_110(~0ULL) == 0ULL);
    // For state = 1ULL << 10:
    // At bit 10: l=0, c=1, r=0 -> (1^0) | (1&1) = 1
    // At bit 11: l=1 (bit 10), c=0, r=0 -> 0
    // At bit 9:  l=0, c=0, r=1 (bit 10) -> (0^1) | 0 = 1
    assert(step_rule_110(1ULL << 10) == ((1ULL << 9) | (1ULL << 10)));
    std::cout << "  • Test 1: Truth table & single-bit asymmetric growth ... [PASS]\n";

    // Test 2: Cyclic shift equivariance across all 64 bit offsets
    uint64_t seed = 0x00010007001F007FULL;
    uint64_t next = step_rule_110(seed);
    for (int k = 0; k < 64; ++k) {
        assert(step_rule_110(std::rotl(seed, k)) == std::rotl(next, k));
    }
    std::cout << "  • Test 2: 64-position cyclic rotation equivariance ... [PASS]\n";

    // Test 3: Exhaustive 8-neighborhood truth table check at bit position 1
    for (uint64_t pat = 0; pat < 8; ++pat) {
        // pat bits: bit 0 = l, bit 1 = c, bit 2 = r
        uint64_t l_bit = (pat >> 0) & 1ULL;
        uint64_t c_bit = (pat >> 1) & 1ULL;
        uint64_t r_bit = (pat >> 2) & 1ULL;
        uint64_t idx = (l_bit << 2) | (c_bit << 1) | r_bit;
        uint64_t expected = (110ULL >> idx) & 1ULL;
        uint64_t out_bit = (step_rule_110(pat) >> 1) & 1ULL;
        assert(out_bit == expected);
    }
    std::cout << "  • Test 3: Exhaustive 8-neighborhood Rule 110 truth table ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 2000000;
    uint64_t state = 0xCAFEBABE12345678ULL;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        state = step_rule_110(state) ^ i;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (state == 0xdeadbeefULL) std::cout << state;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 007 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
