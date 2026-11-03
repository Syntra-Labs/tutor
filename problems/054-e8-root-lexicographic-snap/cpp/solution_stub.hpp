#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

inline constexpr std::array<std::array<int8_t, 8>, 16> CANONICAL_E8_ORBIT_16 = {{
    { 2,  2,  0,  0,  0,  0,  0,  0},
    {-2, -2,  0,  0,  0,  0,  0,  0},
    {-2,  2,  0,  0,  0,  0,  0,  0},
    { 0,  0, -2, -2,  0,  0,  0,  0},
    { 0,  0, -2,  2,  0,  0,  0,  0},
    { 0,  0,  0,  0, -2, -2,  0,  0},
    { 0,  0,  0,  0, -2,  2,  0,  0},
    { 0,  0,  0,  0,  0,  0, -2, -2},
    { 0,  0,  0,  0,  0,  0, -2,  2},
    { 0,  0,  0,  0,  0,  0,  2, -2},
    { 0,  0,  0,  0,  0,  0,  2,  2},
    { 0,  0,  0,  0,  2, -2,  0,  0},
    { 0,  0,  0,  0,  2,  2,  0,  0},
    { 0,  0,  2, -2,  0,  0,  0,  0},
    { 0,  0,  2,  2,  0,  0,  0,  0},
    { 2, -2,  0,  0,  0,  0,  0,  0}
}};

// Compute unscaled L1 distance and coordinate Hamming distance between x[8] and roots[root_idx & 15]
inline void root_cost(
    const std::array<std::array<int8_t, 8>, 16>& roots,
    const int32_t x[8],
    uint8_t root_idx,
    uint32_t& l1_dist,
    uint32_t& hamming
) {
    // TODO: accumulate |x[d] - roots[root_idx & 15][d]| and (x[d] != roots[root_idx & 15][d])
    (void)roots; (void)x; (void)root_idx;
    l1_dist = 0;
    hamming = 0;
}

// Snap x[8] to the root index r in [0, 15] minimizing (l1_dist, hamming, r) against scale * roots[r]
inline uint8_t snap_e8_root(
    const std::array<std::array<int8_t, 8>, 16>& roots,
    const int32_t x[8],
    uint32_t scale = 1
) {
    // TODO: evaluate all 16 scaled roots and return best r under lexicographic (L1, Hamming, r) order
    (void)roots; (void)x; (void)scale;
    return 0;
}
