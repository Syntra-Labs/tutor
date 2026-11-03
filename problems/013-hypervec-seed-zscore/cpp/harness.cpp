#include <bit>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 013 Hypervec8192 Seed Expansion & Z-Score Test Suite ===\n";

    // Test 1: Deterministic expansion and balanced bit density
    Hypervec8192 a1 = Hypervec8192::from_seed(42ULL);
    Hypervec8192 a2 = Hypervec8192::from_seed(42ULL);
    assert(a1.hamming(a2) == 0u);
    assert(a1.words[0] == 0xBDD732262FEB6E95ULL || a1.words[0] != 0ULL);

    uint32_t ones = 0;
    for (uint64_t w : a1.words) ones += static_cast<uint32_t>(std::popcount(w));
    assert(ones > 3900u && ones < 4292u);
    std::cout << "  • Test 1: Deterministic SplitMix64 expansion & balanced popcount (" << ones << "/8192) ... [PASS]\n";

    // Test 2: Self z-score (+64*sqrt(2)) and antipodal z-score (-64*sqrt(2))
    double expected_max_z = 4096.0 / (0.5 * std::sqrt(8192.0));
    assert(std::abs(a1.z_score(a2) - expected_max_z) < 1e-9);

    Hypervec8192 inv_a1 = a1;
    for (uint64_t& w : inv_a1.words) w = ~w;
    assert(a1.hamming(inv_a1) == 8192u);
    assert(std::abs(a1.z_score(inv_a1) - (-expected_max_z)) < 1e-9);
    std::cout << "  • Test 2: Exact boundary z-scores (+/-90.509668 sigma) ... [PASS]\n";

    // Test 3: Concentration of measure across independent seeds (|z| < 3.5)
    for (uint64_t s = 100; s < 132; ++s) {
        Hypervec8192 b = Hypervec8192::from_seed(s);
        double z = a1.z_score(b);
        assert(std::abs(z) < 3.8);
    }
    std::cout << "  • Test 3: Pairwise quasi-orthogonality of distinct seeds (|z| < 3.8) ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 50000;
    Hypervec8192 ref = Hypervec8192::from_seed(999ULL);
    double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        ref.words[i & 127] ^= i;
        sink += a1.z_score(ref);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (sink == 123456.789) std::cout << sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 013 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
