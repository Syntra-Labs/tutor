#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

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

    inline Hypervec8192 rotl(uint32_t shift) const {
        // TODO: implement 8192-bit cross-word cyclic left rotation
        (void)shift;
        return {};
    }

    inline uint32_t hamming(const Hypervec8192& other) const {
        uint32_t d = 0;
        for (size_t i = 0; i < HV_WORDS; ++i) {
            d += static_cast<uint32_t>(std::popcount(words[i] ^ other.words[i]));
        }
        return d;
    }
};
