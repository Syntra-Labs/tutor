#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

struct Hypervec8192 {
    static constexpr size_t BITS = 8192;
    static constexpr size_t WORDS = BITS / 64; // 128 uint64_t words
    std::array<uint64_t, WORDS> words{};
};

// Calculate the bitwise Hamming distance between two 8192-bit hypervectors
inline size_t hamming_distance(const Hypervec8192& a, const Hypervec8192& b) {
    // TODO: Iterate over words, compute XOR and accumulate std::popcount
    (void)a; (void)b;
    return 0;
}

// Calculate the normalized bipolar cosine similarity in range [-1.0, 1.0]
inline double cosine_similarity(const Hypervec8192& a, const Hypervec8192& b) {
    // TODO: Map Hamming distance to [-1.0, 1.0]
    (void)a; (void)b;
    return 0.0;
}
