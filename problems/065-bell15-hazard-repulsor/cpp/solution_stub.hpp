#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

inline constexpr double BELL_COUNTERFACTUAL_SIM_MARGIN = 1.0 / 15.0;

struct RoutedBlockHit {
    uint32_t cell_id = 0;
    uint32_t parent_id = 0;
    uint8_t primary_arch = 0;
    double similarity = 0.0;
};

struct RejectedCellCandidate {
    uint32_t cell_id = 0;
    uint32_t hamming_dist = 4096;
};

struct TaskPlacementResult {
    bool counterfactual_split = false;
    double sim_gap = 1.0;
    uint32_t hazard_cell_id = 0;
    double hazard_similarity = 0.0;
    double hazard_z_score = 0.0;
    std::string decision{};
};

inline TaskPlacementResult evaluate_bell15_and_hazard(
    double lucidity,
    bool has_best_file,
    const std::vector<RoutedBlockHit>& block_hits,
    const std::vector<RejectedCellCandidate>& reject_cells
) {
    // TODO: implement Bell-15 counterfactual task-split detector and +3sigma REJECT hazard repulsor
    (void)lucidity;
    (void)has_best_file;
    (void)block_hits;
    (void)reject_cells;
    return {};
}
