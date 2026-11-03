#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>

#include "solution.hpp"

// Reference BipolarAccumulator to verify 100% bit-exact equivalence
static Hypervec8192 reference_bipolar_encode(
    const std::pair<uint64_t, int16_t>* seeds,
    const size_t* order,
    size_t n
) {
    if (n == 0) return {};
    std::array<int32_t, 8192> counts{};
    for (size_t i = 0; i < n; ++i) {
        size_t idx = order ? order[i] : i;
        uint64_t state = seeds[idx].first;
        int16_t wt = std::clamp<int16_t>(seeds[idx].second, 1, 15);
        for (size_t w = 0; w < 128; ++w) {
            uint64_t word = splitmix64(state);
            for (size_t b = 0; b < 64; ++b) {
                counts[w * 64 + b] += ((word >> b) & 1ULL) ? wt : -wt;
            }
        }
    }
    Hypervec8192 out{};
    for (size_t w = 0; w < 128; ++w) {
        uint64_t word = 0ULL;
        for (size_t b = 0; b < 64; ++b) {
            int32_t c = counts[w * 64 + b];
            bool bit = (b & 1) ? (c >= 0) : (c > 0);
            if (bit) word |= (1ULL << b);
        }
        out.words[w] = word;
    }
    return out;
}

int main() {
    std::cout << "=== Running 018 8-Register Bit-Sliced Vertical Adder Test Suite ===\n";

    // Test 1: Empty input & single seed exact recovery
    Hypervec8192 empty = encode_seeds_bitsliced(nullptr, nullptr, 0);
    for (uint64_t w : empty.words) assert(w == 0ULL);

    std::pair<uint64_t, int16_t> single{0xCAFEBABE1234ULL, 7};
    Hypervec8192 hv_single = encode_seeds_bitsliced(&single, nullptr, 1);
    assert(hv_single.hamming(Hypervec8192::from_seed(single.first)) == 0u);
    std::cout << "  • Test 1: Empty & single-seed identity recovery ... [PASS]\n";

    // Test 2: Bit-exact match with BipolarAccumulator on odd & even total weights (testing tie-breaks)
    std::array<std::pair<uint64_t, int16_t>, 12> seeds{{
        {101ULL, 1}, {202ULL, 4}, {303ULL, 15}, {404ULL, 2},
        {505ULL, 9}, {606ULL, 6}, {707ULL, 3},  {808ULL, 11},
        {909ULL, 5}, {111ULL, 8}, {222ULL, 14}, {333ULL, 7}
    }};

    for (size_t k = 1; k <= 12; ++k) {
        Hypervec8192 bs = encode_seeds_bitsliced(seeds.data(), nullptr, k);
        Hypervec8192 ref = reference_bipolar_encode(seeds.data(), nullptr, k);
        assert(bs.hamming(ref) == 0u);
    }
    std::cout << "  • Test 2: 100% bit-exact equivalence with BipolarAccumulator (k=1..12) ... [PASS]\n";

    // Test 3: Permutation-order indexing via `order` array
    std::array<size_t, 8> order{{7, 2, 5, 0, 11, 3, 9, 1}};
    Hypervec8192 bs_ord = encode_seeds_bitsliced(seeds.data(), order.data(), order.size());
    Hypervec8192 ref_ord = reference_bipolar_encode(seeds.data(), order.data(), order.size());
    assert(bs_ord.hamming(ref_ord) == 0u);
    std::cout << "  • Test 3: Indirect permutation index `order` support ... [PASS]\n";

    // Timing benchmark (16 weighted seeds)
    constexpr size_t ITERS = 10000;
    uint64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        seeds[0].first ^= i;
        Hypervec8192 res = encode_seeds_bitsliced(seeds.data(), nullptr, 12);
        sink ^= res.words[0];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (sink == 0xdeadbeefULL) std::cout << sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 018 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
