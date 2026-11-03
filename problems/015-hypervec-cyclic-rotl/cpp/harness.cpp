#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 015 8192-Bit Cross-Word Cyclic Permutation Test Suite ===\n";

    // Test 1: Single-bit tracking across word boundaries and wrap-around from word 127 to word 0
    Hypervec8192 one{};
    one.words[0] = 1ULL; // bit 0
    Hypervec8192 r63 = one.rotl(63);
    assert(r63.words[0] == (1ULL << 63));
    Hypervec8192 r64 = one.rotl(64);
    assert(r64.words[0] == 0ULL && r64.words[1] == 1ULL);
    Hypervec8192 r8191 = one.rotl(8191);
    assert(r8191.words[127] == (1ULL << 63));
    assert(r8191.rotl(1).hamming(one) == 0u);
    std::cout << "  • Test 1: Single-bit cross-word carry & 8192-bit wrap-around ... [PASS]\n";

    // Test 2: Group homomorphism rotl(a).rotl(b) == rotl(a + b) & popcount conservation
    Hypervec8192 x = Hypervec8192::from_seed(2026ULL);
    uint32_t pop_x = 0;
    for (uint64_t w : x.words) pop_x += static_cast<uint32_t>(std::popcount(w));

    Hypervec8192 step_a = x.rotl(137);
    Hypervec8192 step_ab = step_a.rotl(509);
    Hypervec8192 direct_ab = x.rotl(137 + 509);
    assert(step_ab.hamming(direct_ab) == 0u);
    assert(x.rotl(8192).hamming(x) == 0u);
    assert(step_a.rotl(8192 - 137).hamming(x) == 0u);

    uint32_t pop_rot = 0;
    for (uint64_t w : step_ab.words) pop_rot += static_cast<uint32_t>(std::popcount(w));
    assert(pop_rot == pop_x);
    std::cout << "  • Test 2: Group homomorphism & norm conservation ... [PASS]\n";

    // Test 3: Quasi-orthogonality of shifted vector to unshifted vector & distance isometry
    Hypervec8192 y = Hypervec8192::from_seed(4099ULL);
    uint32_t d_xy = x.hamming(y);
    assert(x.rotl(313).hamming(y.rotl(313)) == d_xy);
    uint32_t d_self_shift = x.hamming(x.rotl(1));
    assert(d_self_shift > 3900u && d_self_shift < 4292u);
    std::cout << "  • Test 3: Shift isometry & 1-bit shift quasi-orthogonality (d_H=" << d_self_shift << ") ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 50000;
    Hypervec8192 cur = x;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        cur = cur.rotl(static_cast<uint32_t>((i & 63) + 1));
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (cur.words[0] == 0xdeadbeefULL) std::cout << cur.words[0];
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 015 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
