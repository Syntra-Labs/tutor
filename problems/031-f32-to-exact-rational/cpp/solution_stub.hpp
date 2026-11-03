#pragma once

#include <bit>
#include <cstdint>
#include <optional>

struct ExactDyadicF32 {
    int64_t m = 0;
    int32_t e = 0;

    constexpr bool operator==(const ExactDyadicF32& o) const = default;
};

constexpr inline std::optional<ExactDyadicF32> decode_f32_bits(uint32_t bits) {
    // TODO:
    // 1. Reject exp_field == 0xff (return std::nullopt)
    // 2. Decode normal ((frac | 0x800000) * 2^(exp - 150)) and subnormal (frac * 2^-149)
    // 3. Canonicalize: if mantissa == 0 return {0, 0}; else shift out trailing zeros so m is odd
    (void)bits;
    return std::nullopt;
}
