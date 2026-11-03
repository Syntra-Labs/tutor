#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <set>
#include <utility>
#include <vector>

using Mat8 = std::array<std::array<int8_t, 8>, 8>;

inline constexpr std::array<std::array<uint8_t, 3>, 7> FANO_CYCLES = {{
    {1, 2, 4}, {2, 3, 5}, {3, 4, 6}, {4, 5, 7},
    {5, 6, 1}, {6, 7, 2}, {7, 1, 3}
}};

inline std::pair<int8_t, uint8_t> octonion_basis_product(uint8_t left, uint8_t right) {
    if (left == 0) return {1, right};
    if (right == 0) return {1, left};
    if (left == right) return {-1, 0};
    for (const auto& cyc : FANO_CYCLES) {
        uint8_t a = cyc[0], b = cyc[1], c = cyc[2];
        if (left == a && right == b) return {1, c};
        if (left == b && right == c) return {1, a};
        if (left == c && right == a) return {1, b};
        if (left == b && right == a) return {-1, c};
        if (left == c && right == b) return {-1, a};
        if (left == a && right == c) return {-1, b};
    }
    return {0, 0};
}

struct SpinorGroup64 {
    std::array<Mat8, 64> states{};
    std::array<std::array<uint8_t, 8>, 64> axes{};
    std::array<std::array<uint8_t, 64>, 64> compose{};
    std::array<uint8_t, 64> inverse{};
    std::array<uint8_t, 15> generators{};
};

// Perform BFS closure from I_8 over the 15 Cl(0,6) bivectors and populate SpinorGroup64
inline SpinorGroup64 build_spinor_group_64() {
    // TODO: implement BFS closure, axes compression, compose[64][64], inverse[64], generators[15]
    return SpinorGroup64{};
}

// Apply compressed signed-permutation operator state_id to 8D vector x -> y
inline void spinor_action(const SpinorGroup64& grp, uint8_t state_id, const int32_t x[8], int32_t y[8]) {
    // TODO: use grp.axes[state_id & 63] to permute and sign-flip x into y
    (void)grp; (void)state_id; (void)x; (void)y;
}
