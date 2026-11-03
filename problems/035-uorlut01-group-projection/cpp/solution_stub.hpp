#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

using ActivationLUT = std::array<int64_t, 16>;

inline void build_activation_luts(const int32_t* x, size_t d_in, ActivationLUT* luts_out) {
    // TODO: For each j in 0..d_in, fill luts_out[j][u] = (u - 8) * x[j] for u in 0..15
    // using only additions/subtractions
    (void)x;
    (void)d_in;
    (void)luts_out;
}

inline void uorlut01_project(
    const uint8_t* packed_weights,
    const uint8_t* group_scales,
    const int32_t* x,
    size_t d_out,
    size_t d_in,
    int64_t* y_out
) {
    // TODO: Precompute ActivationLUTs for x, then for each row i in 0..d_out and group g in 0..(d_in/32):
    //   1. Unpack 32 4-bit weight codes u in 0..15 from 16 bytes (low nibble = 2k, high nibble = 2k+1)
    //   2. Sum luts[32*g + c][u]
    //   3. Apply group scale byte s_g = m * 2^e (m = scale & 0x0F, e = (scale >> 4) & 0x0F) using shifts/adds
    (void)packed_weights;
    (void)group_scales;
    (void)x;
    (void)d_out;
    (void)d_in;
    (void)y_out;
}
