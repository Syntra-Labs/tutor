#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

struct E8CarrierAddress {
    int32_t first_block[8]{};
    int32_t last_block[8]{};
    std::array<uint8_t, 4> block_roots{};
    uint8_t admission_cell = 0;
};

// The 16 canonical integer frame roots in R^8 (scaled by 2 for exact integer arithmetic)
inline constexpr int32_t E8_ORBIT_ROOTS[16][8] = {
    { 2,  2,  0,  0,  0,  0,  0,  0},
    { 2, -2,  0,  0,  0,  0,  0,  0},
    {-2,  2,  0,  0,  0,  0,  0,  0},
    {-2, -2,  0,  0,  0,  0,  0,  0},
    { 0,  0,  2,  2,  0,  0,  0,  0},
    { 0,  0,  2, -2,  0,  0,  0,  0},
    { 0,  0, -2,  2,  0,  0,  0,  0},
    { 0,  0, -2, -2,  0,  0,  0,  0},
    { 0,  0,  0,  0,  2,  2,  0,  0},
    { 0,  0,  0,  0,  2, -2,  0,  0},
    { 0,  0,  0,  0, -2,  2,  0,  0},
    { 0,  0,  0,  0, -2, -2,  0,  0},
    { 1,  1,  1,  1,  1,  1,  1,  1},
    { 1,  1,  1,  1, -1, -1, -1, -1},
    { 1, -1,  1, -1,  1, -1,  1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1}
};

inline uint32_t e8_root_l1_cost(const int32_t block[8], uint8_t root_idx) {
    uint32_t cost = 0;
    for (size_t d = 0; d < 8; ++d) {
        int32_t diff = block[d] - E8_ORBIT_ROOTS[root_idx][d];
        cost += static_cast<uint32_t>(diff < 0 ? -diff : diff);
    }
    return cost;
}

// Compute the 256 product-cell L1 costs and return the boolean admission mask for the top K=32 cells.
inline std::array<bool, 256> compute_e8_shortlist_admission(
    const E8CarrierAddress& query_addr,
    std::array<uint32_t, 256>* out_cell_costs = nullptr
) {
    // TODO: implement 16x16 product-cell L1 cost evaluation and top-32 partial sort admission gate
    (void)query_addr;
    (void)out_cell_costs;
    std::array<bool, 256> admitted{};
    return admitted;
}
