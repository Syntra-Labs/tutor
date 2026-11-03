#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 019 256-Bit Helix Quadrant Signature Test Suite ===\n";

    // Test 1: All-zero and all-one hypervectors
    Hypervec8192 zeros{};
    HelixSignature256 sig_zero = compute_helix_signature(zeros);
    assert(sig_zero.s0 == 0ULL && sig_zero.s1 == 0ULL && sig_zero.s2 == 0ULL && sig_zero.s3 == 0ULL);

    Hypervec8192 ones{};
    ones.words.fill(~0ULL);
    HelixSignature256 sig_ones = compute_helix_signature(ones);
    assert(sig_ones.s0 == ~0ULL && sig_ones.s1 == ~0ULL && sig_ones.s2 == ~0ULL && sig_ones.s3 == ~0ULL);
    assert(sig_zero.hamming(sig_ones) == 256u);
    std::cout << "  • Test 1: Boundary all-0 and all-1 hypervector signatures ... [PASS]\n";

    // Test 2: Exact 64-bit tie-breaking across all 64 pairs in s0
    Hypervec8192 tied{};
    for (size_t b = 0; b < 64; ++b) {
        tied.words[2 * b] = ~0ULL;     // 64 ones
        tied.words[2 * b + 1] = 0ULL;  // 0 ones -> total 64 ones in pair!
    }
    HelixSignature256 sig_tied = compute_helix_signature(tied);
    assert(sig_tied.s0 == 0xAAAAAAAAAAAAAAAAULL);
    assert(sig_tied.s1 == ~0ULL && sig_tied.s2 == 0ULL && sig_tied.s3 == ~0ULL);
    std::cout << "  • Test 2: Exact 64-one pair tie-breaker (s0 == 0xAAAAAAAAAAAAAAAA) ... [PASS]\n";

    // Test 3: Sensitivity of s0 to mutations in high words (e.g. words 100..127)
    Hypervec8192 hv = Hypervec8192::from_seed(7777ULL);
    HelixSignature256 s_orig = compute_helix_signature(hv);
    assert(s_orig.s1 == hv.words[0] && s_orig.s2 == hv.words[1] && s_orig.s3 == hv.words[2]);

    Hypervec8192 hv_tail_flip = hv;
    for (size_t w = 64; w < 128; ++w) hv_tail_flip.words[w] = ~hv_tail_flip.words[w];
    HelixSignature256 s_flip = compute_helix_signature(hv_tail_flip);
    // Prefix words s1..s3 unchanged, upper 32 bits of s0 flipped (except possible 64-ties)
    assert(s_flip.s1 == s_orig.s1 && s_flip.s2 == s_orig.s2 && s_flip.s3 == s_orig.s3);
    assert(std::popcount((s_flip.s0 ^ s_orig.s0) >> 32) >= 24);
    std::cout << "  • Test 3: Global coverage of tail words (64..127) in s0 ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 100000;
    uint32_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        hv.words[i & 127] ^= i;
        HelixSignature256 s = compute_helix_signature(hv);
        sink += s.hamming(s_orig);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (sink == 0xdeadbeefu) std::cout << sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 019 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
