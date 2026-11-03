#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <unordered_set>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 005 FNV-1a & SplitMix64 Test Suite ===\n";

    // Test 1: Known FNV-1a 64-bit test vectors
    assert(fnv1a_str("") == 14695981039346656037ULL);
    assert(fnv1a_str("a") == 12638187200555641996ULL);
    assert(fnv1a_str("foobar") == 0x85944171f73967e8ULL);
    std::cout << "  • Test 1: Known FNV-1a 64-bit standard vectors ... [PASS]\n";

    // Test 2: Known SplitMix64 sequence from state = 0 and bijectivity check
    uint64_t state = 0ULL;
    uint64_t out1 = splitmix64(state);
    uint64_t out2 = splitmix64(state);
    assert(state == 2ULL * 0x9E3779B97F4A7C15ULL);
    assert(out1 == 0xE220A8397B1DCDAFULL);
    assert(out2 == 0x6E789E6AA1B965F4ULL);

    std::unordered_set<uint64_t> seen;
    seen.reserve(4096);
    uint64_t s_bij = 42ULL;
    for (int i = 0; i < 4096; ++i) {
        assert(seen.insert(splitmix64(s_bij)).second);
    }
    std::cout << "  • Test 2: SplitMix64 reference vectors & collision-free bijection ... [PASS]\n";

    // Test 3: Avalanche bit-dispersion check (~32 bits flipped per single-bit input flip)
    uint64_t total_flipped = 0;
    for (int b = 0; b < 64; ++b) {
        uint64_t s0 = 0x123456789ABCDEF0ULL;
        uint64_t s1 = s0 ^ (1ULL << b);
        uint64_t h0 = splitmix64(s0);
        uint64_t h1 = splitmix64(s1);
        total_flipped += static_cast<uint64_t>(std::popcount(h0 ^ h1));
    }
    double avg_flipped = static_cast<double>(total_flipped) / 64.0;
    assert(avg_flipped >= 26.0 && avg_flipped <= 38.0);
    std::cout << "  • Test 3: Avalanche dispersion (avg " << avg_flipped << " bits flipped) ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 1000000;
    uint64_t bench_state = 1ULL;
    uint64_t sink = 0ULL;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        sink ^= splitmix64(bench_state);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (sink == 0xdeadbeefULL) std::cout << sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 005 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
