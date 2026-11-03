#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

struct RankMarginResult {
    int32_t T = 0;            // Token margin shortfall: max(0, 32 + d_target - d_rival)
    int32_t A = 0;            // Address margin shortfall: max(0, 8 + addr_target - addr_rival_cell)
    int32_t occupied_rank = 1;// 1-based rank of target_cell among occupied cells by (addr_dist, cell_id)
    int32_t R = 0;            // Rank hinge shortfall: max(0, occupied_rank - K)
    int32_t loss_A_num = 0;   // (T << 3) + (A << 5)
    int32_t loss_R_num = 0;   // (T << 3) + (R << 5)
};

inline RankMarginResult evaluate_rank_margin_hinge(
    int32_t d_target,
    int32_t d_best_rival,
    int32_t addr_target,
    int32_t addr_best_rival_cell,
    const std::array<int32_t, 256>& cell_addr_dist,
    const std::array<bool, 256>& cell_occupied,
    uint8_t target_cell,
    int32_t K
) {
    // TODO: implement multiplier-free integer Rank-Margin Hinge Objectives A and R
    (void)d_target;
    (void)d_best_rival;
    (void)addr_target;
    (void)addr_best_rival_cell;
    (void)cell_addr_dist;
    (void)cell_occupied;
    (void)target_cell;
    (void)K;
    return {};
}
