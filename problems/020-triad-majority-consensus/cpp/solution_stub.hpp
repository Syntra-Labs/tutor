#pragma once

#include <bit>
#include <cstdint>

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

// Compute 3-way bitwise majority signature across three HelixSignature256 blocks
constexpr HelixSignature256 triad_majority_signature(
    const HelixSignature256& a,
    const HelixSignature256& b,
    const HelixSignature256& c
) {
    // TODO: implement (a & b) | (b & c) | (a & c) across all 4 words
    (void)a;
    (void)b;
    (void)c;
    return {};
}
