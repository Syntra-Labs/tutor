#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <utility>

inline uint64_t splitmix64(uint64_t& state) {
    uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

struct Hypervec8192 {
    static constexpr size_t HV_BITS = 8192;
    static constexpr size_t HV_WORDS = 128;
    std::array<uint64_t, HV_WORDS> words{};

    static inline Hypervec8192 from_seed(uint64_t seed) {
        Hypervec8192 hv;
        uint64_t state = seed;
        for (size_t i = 0; i < HV_WORDS; ++i) hv.words[i] = splitmix64(state);
        return hv;
    }

    inline uint32_t hamming(const Hypervec8192& other) const {
        uint32_t d = 0;
        for (size_t i = 0; i < HV_WORDS; ++i) {
            d += static_cast<uint32_t>(std::popcount(words[i] ^ other.words[i]));
        }
        return d;
    }
};

// Encode n weighted seeds into an 8192-bit hypervector using an 8-register vertical carry-save counter
inline Hypervec8192 encode_seeds_bitsliced(
    const std::pair<uint64_t, int16_t>* seeds,
    const size_t* order,
    size_t n
) {
    // TODO: implement pre-biased 8-bit vertical ripple-carry accumulator across 8 registers (b0..b7)
    (void)seeds;
    (void)order;
    (void)n;
    return {};
}
