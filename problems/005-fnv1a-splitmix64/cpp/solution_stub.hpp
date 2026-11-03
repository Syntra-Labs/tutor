#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

static constexpr uint64_t FNV1A_64_BASIS = 14695981039346656037ULL;
static constexpr uint64_t FNV1A_64_PRIME = 1099511628211ULL;

// Compute 64-bit FNV-1a hash over a raw byte buffer
inline uint64_t fnv1a_64(const void* data, size_t len, uint64_t basis = FNV1A_64_BASIS) {
    // TODO: implement FNV-1a byte-by-byte XOR then multiply by FNV1A_64_PRIME
    (void)data;
    (void)len;
    return basis;
}

// Compute 64-bit FNV-1a hash over a string_view
inline uint64_t fnv1a_str(std::string_view s, uint64_t basis = FNV1A_64_BASIS) {
    return fnv1a_64(s.data(), s.size(), basis);
}

// Advance state by golden ratio increment and return Stafford Mix13 bijective hash
inline uint64_t splitmix64(uint64_t& state) {
    // TODO: implement SplitMix64 state increment (0x9E3779B97F4A7C15ULL) and Mix13 finalizer
    (void)state;
    return 0;
}
