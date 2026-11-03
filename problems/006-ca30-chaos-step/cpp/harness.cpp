#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 006 CA Rule 30 Step Test Suite ===\n";

    // Test 1: Absorbing states and single-bit seed expansion
    assert(step_rule_30(0ULL) == 0ULL);
    assert(step_rule_30(~0ULL) == 0ULL);
    // For state = 1 (bit 0 set):
    // l has bit 1 set (2), r has bit 63 set (1ULL << 63), c has bit 0 set (1).
    // l ^ (c | r) => bits 0, 1, and 63 set!
    uint64_t s1 = step_rule_30(1ULL);
    assert(s1 == (1ULL | 2ULL | (1ULL << 63)));
    std::cout << "  • Test 1: Fixed points & single-bit light-cone expansion ... [PASS]\n";

    // Test 2: Cyclic shift equivariance: step(rotl(x, k)) == rotl(step(x), k)
    uint64_t pattern = 0xD3ADB33FCAFEF00DULL;
    uint64_t stepped = step_rule_30(pattern);
    for (int k = 0; k < 64; ++k) {
        assert(step_rule_30(std::rotl(pattern, k)) == std::rotl(stepped, k));
    }
    std::cout << "  • Test 2: 64-position cyclic rotation equivariance ... [PASS]\n";

    // Test 3: Multi-step chaotic trajectory from seed = 1ULL << 32
    uint64_t x = 1ULL << 32;
    x = step_rule_30(x); // t=1: bits 31, 32, 33 -> 0b111 << 31
    assert(x == (0b111ULL << 31));
    x = step_rule_30(x); // t=2: 11001 in Rule 30 pyramid centered at 32 -> bits 30..34 = 10011_2 (since l=bit-1, r=bit+1)
    assert(std::popcount(x) == 3);
    std::cout << "  • Test 3: Multi-step Rule 30 light-cone trajectory ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 2000000;
    uint64_t state = 0x123456789ABCDEF1ULL;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        state = step_rule_30(state) ^ i;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (state == 0xdeadbeefULL) std::cout << state;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 006 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
