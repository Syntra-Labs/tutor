#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 065 Bell-15 Counterfactual & Hazard Repulsor Test Suite ===\n";

    // Test 1: Distinct file within 1/15 similarity gap triggers counterfactual_split
    {
        std::vector<RoutedBlockHit> hits = {
            {101, 10, 2, 0.580},
            {202, 20, 2, 0.540} // different parent_id (20 != 10), sim >= 0.525, gap = 0.04 < 0.06667
        };
        std::vector<RejectedCellCandidate> rejects = {
            {901, 4096}, // z = 0.0, sim = 0.5
            {902, 3914}  // z = (4096 - 3914) / 45.254834 = +4.02167
        };
        auto res = evaluate_bell15_and_hazard(0.72, true, hits, rejects);
        assert(res.counterfactual_split == true);
        assert(std::abs(res.sim_gap - 0.04) < 1e-9);
        assert(res.hazard_cell_id == 902);
        assert(res.hazard_z_score > 4.0);
        assert(res.decision == "INSERT_AT_EXISTING_BLOCK");
        std::cout << "  • Test 1: Bell-15 multi-concern split and +4sigma hazard detection ... [PASS]\n";
    }

    // Test 2: Same file AND same archetype does NOT trigger counterfactual split
    {
        std::vector<RoutedBlockHit> hits = {
            {101, 10, 2, 0.580},
            {102, 10, 2, 0.560} // same parent_id=10 and same primary_arch=2
        };
        auto res = evaluate_bell15_and_hazard(0.45, true, hits, {});
        assert(res.counterfactual_split == false);
        assert(res.decision == "SCAFFOLD_NEW_BLOCK_IN_FILE");
        std::cout << "  • Test 2: Same-concern runner-up suppresses false split ... [PASS]\n";
    }

    // Test 3: Gap >= 1/15 or runner-up < 0.525 suppresses split; no file -> SCAFFOLD_NEW_FILE_FOR_STAGE
    {
        std::vector<RoutedBlockHit> hits = {
            {101, 10, 1, 0.620},
            {202, 20, 3, 0.540} // gap = 0.08 > 1/15
        };
        auto res = evaluate_bell15_and_hazard(0.30, false, hits, {});
        assert(res.counterfactual_split == false);
        assert(res.decision == "SCAFFOLD_NEW_FILE_FOR_STAGE");
        std::cout << "  • Test 3: Gap >= 1/15 threshold and new-file scaffolding decision ... [PASS]\n";
    }

    // Performance benchmark
    std::vector<RoutedBlockHit> bench_hits = {{1, 1, 1, 0.59}, {2, 2, 1, 0.55}};
    std::vector<RejectedCellCandidate> bench_rej(64);
    for (uint32_t i = 0; i < 64; ++i) {
        bench_rej[i] = {1000 + i, 4050 - i * 2};
    }
    constexpr size_t ITERS = 50000;
    double acc = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        bench_hits[0].similarity = 0.56 + 0.001 * static_cast<double>(i & 31);
        auto r = evaluate_bell15_and_hazard(0.68, true, bench_hits, bench_rej);
        acc += r.hazard_z_score + (r.counterfactual_split ? 1.0 : 0.0);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (acc == -1.0) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
