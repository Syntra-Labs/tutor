#pragma once

#include <array>
#include <cstdint>

constexpr inline uint64_t isqrt_u64(uint64_t n) {
    // TODO: Compute exact floor(sqrt(n)) for any uint64_t using bitwise digit-by-digit method
    // with zero floating-point operations
    (void)n;
    return 0;
}

constexpr inline uint64_t lut_mul_u64(uint32_t a, uint32_t b) {
    // TODO: Build T[16] where T[q] = q * a using only additions, then accumulate
    // sum_{k=0..7} (T[(b >> (4*k)) & 15] << (4*k)) with zero '*' instructions
    (void)a;
    (void)b;
    return 0;
}
