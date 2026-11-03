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

struct E8Orbit16 {
    std::array<std::array<uint8_t, 8>, 64> axes{};
    std::array<std::array<uint8_t, 64>, 64> compose{};
    std::array<uint8_t, 64> inverse{};
    std::array<std::array<int8_t, 8>, 16> roots{};
    std::array<std::array<uint8_t, 16>, 16> bfs{};

    void action(uint8_t state_id, const int32_t x[8], int32_t y[8]) const {
        const auto& ax = axes[state_id & 63];
        for (size_t d = 0; d < 8; ++d) {
            uint8_t a = ax[d];
            int32_t v = x[a & 7];
            y[d] = (a & 8) ? -v : v;
        }
    }
};

// Build the 64-state spinor group, 16-root E8 orbit from anchor (2,2,0,0,0,0,0,0), and 16x16 BFS transport table
inline E8Orbit16 build_e8_orbit_16() {
    // TODO: implement orbit generation and 16x16 BFS transport table matching SpiralCoreGeometry
    return E8Orbit16{};
}
