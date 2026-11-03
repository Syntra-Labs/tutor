#pragma once

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <string_view>

struct ExtractedPatch {
    uint64_t seed;
    int16_t intrinsic_weight;
    bool is_primary_token;
};

constexpr inline uint64_t fnv1a_64(
    const char* data,
    size_t len,
    uint64_t basis = 0xCBF29CE484222325ULL
) {
    uint64_t h = basis;
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<uint8_t>(data[i]);
        h *= 0x100000001B3ULL;
    }
    return h;
}

constexpr inline uint64_t make_bigram_seed(uint64_t prev1_primary, uint64_t cur_primary) {
    // TODO: std::rotl(prev1_primary, 3) ^ (cur_primary * 0x9E3779B97F4A7C15ULL)
    (void)prev1_primary;
    (void)cur_primary;
    return 0;
}

constexpr inline uint64_t make_skipgram_seed(uint64_t prev2_primary, uint64_t cur_primary) {
    // TODO: std::rotl(prev2_primary, 5) ^ (cur_primary * 0xBF58476D1CE4E5B9ULL)
    (void)prev2_primary;
    (void)cur_primary;
    return 0;
}

inline size_t extract_patches(std::string_view text, ExtractedPatch out_patches[128]) {
    // TODO: Split identifiers & CamelCase words, emit primary tokens, 4-gram subwords,
    // and 3-char prefixes into out_patches (up to 120 patches) and return n_patches.
    (void)text;
    (void)out_patches;
    return 0;
}
