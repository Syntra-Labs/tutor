#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 008 CA 184/30 Counterfactual Demarcator Test Suite ===\n";

    // Test 1: Uniform states (0 and ~0) have 0 spatial transitions and conserve particles
    bool conserved = false;
    uint32_t d0 = count_demarcs(0ULL, conserved);
    assert(conserved);
    assert(d0 == 0u);

    conserved = false;
    uint32_t d_all = count_demarcs(~0ULL, conserved);
    assert(conserved);
    assert(d_all == 0u);
    std::cout << "  • Test 1: Homogeneous states yield 0 demarcations & conserve particles ... [PASS]\n";

    // Test 2: High-frequency alternating traffic (0x5555555555555555ULL)
    // For 0x5555...: l = 0xAAAA..., r = 0xAAAA...
    // fwd_1 = (l & ~c) | (c & r) = 0xAAAA...
    // cf_1  = step_184(0xAAAA...) = 0x5555...
    // r30_1 = 0xAAAA ^ (0x5555 | 0xAAAA) = 0xAAAA ^ 0xFFFF = 0x5555...
    // Notice fwd_1 == ~r30_1 (both 0xAAAA) and cf_1 == r30_1 (both 0x5555)!
    // So matches == 0xFFFFFFFFFFFFFFFFULL -> demarcs == 0!
    conserved = false;
    uint32_t d_alt = count_demarcs(0x5555555555555555ULL, conserved);
    assert(conserved);
    assert(d_alt == 0u);
    std::cout << "  • Test 2: Alternating crystal lattice exact phase lock ... [PASS]\n";

    // Test 3: Heterogeneous seeds satisfy dual conservation, even demarcation parity, and rotation invariance
    uint64_t seed = 0xF0F0AA55CC330F0FULL;
    conserved = false;
    uint32_t d_seed = count_demarcs(seed, conserved);
    assert(conserved);
    assert(d_seed > 0u && (d_seed % 2u == 0u));

    for (int k = 1; k < 64; ++k) {
        bool c_rot = false;
        uint32_t d_rot = count_demarcs(std::rotl(seed, k), c_rot);
        assert(c_rot);
        assert(d_rot == d_seed);
    }
    std::cout << "  • Test 3: Dual conservation, even ring parity & shift invariance ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 1000000;
    uint64_t acc = 0;
    bool c_flag = false;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        acc += count_demarcs(seed ^ i, c_flag);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (acc == 0xdeadbeefULL) std::cout << acc;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 008 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
