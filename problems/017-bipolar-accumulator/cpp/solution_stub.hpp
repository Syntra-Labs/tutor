#pragma once

#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>

inline uint64_t splitmix64(uint64_t& state) {
    uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

struct BipolarByteLUT {
    alignas(32) int16_t table[256][8]{};
    constexpr BipolarByteLUT() {
        for (int b = 0; b < 256; ++b) {
            for (int k = 0; k < 8; ++k) {
                table[b][k] = ((b >> k) & 1) ? 1 : -1;
            }
        }
    }
};

static constexpr BipolarByteLUT BYTE_BIPOLAR_LUT{};

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

    inline double z_score(const Hypervec8192& other) const {
        return (4096.0 - static_cast<double>(hamming(other))) / (0.5 * std::sqrt(8192.0));
    }
};

struct BipolarAccumulator {
    alignas(64) std::array<int16_t, Hypervec8192::HV_BITS> counts{};
    uint32_t total_items = 0;

    inline void clear() {
        counts.fill(0);
        total_items = 0;
    }

    // Accumulate weighted hypervector into counts[0..8191] using BYTE_BIPOLAR_LUT
    inline void accumulate(const Hypervec8192& hv, int16_t weight = 1) {
        // TODO: implement LUT-accelerated bipolar accumulation
        (void)hv;
        (void)weight;
    }

    // Project counts[0..8191] to Hypervec8192 using >0 on even bits and >=0 on odd bits
    inline Hypervec8192 project() const {
        // TODO: implement unrolled 8-bit group projection with alternating tie-breaker
        return {};
    }
};
