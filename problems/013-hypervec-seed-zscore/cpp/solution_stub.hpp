#pragma once

#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>

inline uint64_t splitmix64(uint64_t& state) {
    // TODO: implement SplitMix64 state increment and Mix13 finalizer
    (void)state;
    return 0;
}

struct Hypervec8192 {
    static constexpr size_t HV_BITS = 8192;
    static constexpr size_t HV_WORDS = 128;
    std::array<uint64_t, HV_WORDS> words{};

    static inline Hypervec8192 from_seed(uint64_t seed) {
        // TODO: expand 64-bit seed into 128 words via splitmix64
        (void)seed;
        return {};
    }

    inline uint32_t hamming(const Hypervec8192& other) const {
        // TODO: return bitwise Hamming distance across 128 words
        (void)other;
        return 0;
    }

    inline double z_score(const Hypervec8192& other) const {
        // TODO: return (4096.0 - hamming(other)) / (0.5 * sqrt(8192.0))
        (void)other;
        return 0.0;
    }
};
