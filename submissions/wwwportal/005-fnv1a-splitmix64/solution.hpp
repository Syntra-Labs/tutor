#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

static constexpr uint64_t FNV1A_64_BASIS = 14695981039346656037ULL;
static constexpr uint64_t FNV1A_64_PRIME = 1099511628211ULL;

inline uint64_t fnv1a_64(const void* data, size_t len, uint64_t basis = FNV1A_64_BASIS) {
    const auto* bytes = static_cast<const uint8_t*>(data);
    uint64_t h = basis;
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<uint64_t>(bytes[i]);
        h *= FNV1A_64_PRIME;
    }
    return h;
}

inline uint64_t fnv1a_str(std::string_view s, uint64_t basis = FNV1A_64_BASIS) {
    return fnv1a_64(s.data(), s.size(), basis);
}

inline uint64_t splitmix64(uint64_t& state) {
    uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}
