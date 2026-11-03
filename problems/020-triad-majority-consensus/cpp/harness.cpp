#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 020 3-Block Triad Majority Consensus Test Suite ===\n";

    HelixSignature256 A{0x5555555555555555ULL, 0xF0F0F0F0F0F0F0F0ULL, 0x123456789ABCDEF0ULL, 0x0000FFFFFFFF0000ULL};
    HelixSignature256 B{0x3333333333333333ULL, 0xFF00FF00FF00FF00ULL, 0x0FEDCBA987654321ULL, 0xFFFF0000FFFF0000ULL};
    HelixSignature256 C{0x0F0F0F0F0F0F0F0FULL, 0xAAAA5555AAAA5555ULL, 0xDEADBEEFCAFEBABEULL, 0x00000000FFFFFFFFULL};

    // Test 1: Unanimity & 2-of-3 dominance law: maj(A, A, B) == A
    HelixSignature256 aab = triad_majority_signature(A, A, B);
    assert(aab.hamming(A) == 0u);
    HelixSignature256 aba = triad_majority_signature(A, B, A);
    assert(aba.hamming(A) == 0u);
    HelixSignature256 baa = triad_majority_signature(B, A, A);
    assert(baa.hamming(A) == 0u);
    std::cout << "  • Test 1: 2-of-3 quorum dominance maj(A, A, B) == A ... [PASS]\n";

    // Test 2: Steiner median identity on hypercube: d(M,A)+d(M,B)+d(M,C) == (d(A,B)+d(B,C)+d(A,C))/2
    HelixSignature256 M = triad_majority_signature(A, B, C);
    uint32_t d_MA = M.hamming(A);
    uint32_t d_MB = M.hamming(B);
    uint32_t d_MC = M.hamming(C);
    uint32_t sum_to_median = d_MA + d_MB + d_MC;
    uint32_t sum_pairwise = A.hamming(B) + B.hamming(C) + A.hamming(C);

    assert(2u * sum_to_median == sum_pairwise);
    assert(sum_to_median <= 256u);
    std::cout << "  • Test 2: Exact hypercube Steiner median identity (sum=" << sum_to_median << " <= 256) ... [PASS]\n";

    // Test 3: Two-child proximity guarantee (at least 2 children within Hamming distance <= 128) & symmetry
    int within_128 = (d_MA <= 128u ? 1 : 0) + (d_MB <= 128u ? 1 : 0) + (d_MC <= 128u ? 1 : 0);
    assert(within_128 >= 2);
    HelixSignature256 M_perm = triad_majority_signature(C, A, B);
    assert(M.hamming(M_perm) == 0u);
    std::cout << "  • Test 3: Two-child <= 128 proximity guarantee & permutation invariance ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 2000000;
    HelixSignature256 acc = A;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        acc.s0 ^= i;
        acc = triad_majority_signature(acc, B, C);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (acc.s0 == 0xdeadbeefULL) std::cout << acc.s1;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 020 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
