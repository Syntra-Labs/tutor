#include <bit>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 017 Bipolar Accumulator & Projection Test Suite ===\n";

    BipolarAccumulator acc;
    acc.clear();

    // Test 1: Zero-state alternating tie-break projects to 0xAAAAAAAAAAAAAAAAULL (4096 ones)
    Hypervec8192 empty_proj = acc.project();
    uint32_t empty_ones = 0;
    for (uint64_t w : empty_proj.words) {
        assert(w == 0xAAAAAAAAAAAAAAAAULL);
        empty_ones += static_cast<uint32_t>(std::popcount(w));
    }
    assert(empty_ones == 4096u);
    std::cout << "  • Test 1: Unbiased alternating tie-break (4096/8192 ones on tie) ... [PASS]\n";

    // Test 2: Single vector accumulation & exact negative-weight cancellation
    Hypervec8192 v1 = Hypervec8192::from_seed(101ULL);
    Hypervec8192 v2 = Hypervec8192::from_seed(202ULL);
    Hypervec8192 v3 = Hypervec8192::from_seed(303ULL);

    acc.accumulate(v1, 3);
    assert(acc.total_items == 3u);
    assert(acc.project().hamming(v1) == 0u);

    acc.accumulate(v2, 5);
    assert(acc.total_items == 8u);
    assert(acc.project().hamming(v2) == 0u); // weight 5 > weight 3 everywhere

    acc.accumulate(v2, -5); // exact unbundle of v2
    assert(acc.total_items == 3u);
    assert(acc.project().hamming(v1) == 0u);
    std::cout << "  • Test 2: Weighted dominance & exact negative-weight unbundling ... [PASS]\n";

    // Test 3: 3-vector majority bundle matches bitwise majority & achieves high z-score to all 3
    acc.clear();
    acc.accumulate(v1, 1);
    acc.accumulate(v2, 1);
    acc.accumulate(v3, 1);
    Hypervec8192 bundle = acc.project();

    for (size_t w = 0; w < Hypervec8192::HV_WORDS; ++w) {
        uint64_t maj = (v1.words[w] & v2.words[w]) |
                       (v2.words[w] & v3.words[w]) |
                       (v1.words[w] & v3.words[w]);
        assert(bundle.words[w] == maj);
    }
    assert(bundle.z_score(v1) > 40.0);
    assert(bundle.z_score(v2) > 40.0);
    assert(bundle.z_score(v3) > 40.0);
    std::cout << "  • Test 3: Exact 3-vector majority equivalence & >40 sigma recall ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 20000;
    uint32_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        acc.accumulate(v1, 1);
    }
    Hypervec8192 p = acc.project();
    auto t1 = std::chrono::high_resolution_clock::now();
    sink = p.words[0] & 1u;
    if (sink == 0xdeadbeefu) std::cout << sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 017 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
