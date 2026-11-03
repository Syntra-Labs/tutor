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
};

struct HelixSignature256 {
    uint64_t s0 = 0;
    uint64_t s1 = 0;
    uint64_t s2 = 0;
    uint64_t s3 = 0;

    inline uint32_t hamming(const HelixSignature256& other) const {
        // TODO: return 256-bit Hamming distance across s0, s1, s2, s3
        (void)other;
        return 0;
    }
};

// Compute 256-bit multi-resolution Helix signature from an 8192-bit hypervector
inline HelixSignature256 compute_helix_signature(const Hypervec8192& hv) {
    // TODO: compute coarse global summary word s0 and prefix words s1..s3
    (void)hv;
    return {};
}
