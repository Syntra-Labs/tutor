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
    size_t dist = 0;
    for (size_t i = 0; i < Hypervec8192::WORDS; ++i) {
        dist += std::popcount(a.words[i] ^ b.words[i]);
    }
    return dist;
}

// Calculate the normalized bipolar cosine similarity in range [-1.0, 1.0]
inline double cosine_similarity(const Hypervec8192& a, const Hypervec8192& b) {
    size_t d = hamming_distance(a, b);
    return 1.0 - (2.0 * static_cast<double>(d) / static_cast<double>(Hypervec8192::BITS));
}
