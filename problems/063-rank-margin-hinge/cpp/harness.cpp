#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 063 Integer Rank-Margin Hinge Test Suite ===\n";

    std::array<int32_t, 256> cell_dist{};
    std::array<bool, 256> occupied{};
    for (size_t c = 0; c < 256; ++c) {
        cell_dist[c] = static_cast<int32_t>(c * 2);
        occupied[c] = (c % 2 == 0); // 128 occupied cells: 0, 2, 4, ..., 254
    }

    // Test 1: Target beats rival by >= margin -> zero loss
    {
        auto res = evaluate_rank_margin_hinge(10, 42, 4, 12, cell_dist, occupied, 4, 16);
        assert(res.T == 0);
        assert(res.A == 0);
        assert(res.occupied_rank == 3); // cells 0, 2 precede cell 4
        assert(res.R == 0);             // 3 <= 16
        assert(res.loss_A_num == 0);
        assert(res.loss_R_num == 0);
        std::cout << "  • Test 1: Margin satisfied -> zero loss for A and R ... [PASS]\n";
    }

    // Test 2: Positive shortfalls T and A, shift-weighted combination
    {
        // d_target = 20, d_rival = 30 -> T = max(0, 32 + 20 - 30) = 22
        // addr_target = 10, addr_rival = 12 -> A = max(0, 8 + 10 - 12) = 6
        // target_cell = 40 (occupied: 0, 2, ..., 38 precede it -> 20 cells precede -> rank = 21)
        // K = 16 -> R = 21 - 16 = 5
        auto res = evaluate_rank_margin_hinge(20, 30, 10, 12, cell_dist, occupied, 40, 16);
        assert(res.T == 22);
        assert(res.A == 6);
        assert(res.occupied_rank == 21);
        assert(res.R == 5);
        assert(res.loss_A_num == (22 << 3) + (6 << 5));
        assert(res.loss_R_num == (22 << 3) + (5 << 5));
        std::cout << "  • Test 2: Exact shift-weighted Objective A & R numerators ... [PASS]\n";
    }

    // Test 3: Tie-breaking by cell index when address distances are equal
    {
        std::array<int32_t, 256> flat_dist{};
        flat_dist.fill(10);
        std::array<bool, 256> occ{};
        occ[5] = true;
        occ[10] = true;
        occ[15] = true;
        occ[20] = true;
        // Unoccupied cells with smaller index (e.g. 0..4) must NOT count!
        auto res = evaluate_rank_margin_hinge(50, 50, 10, 10, flat_dist, occ, 15, 2);
        assert(res.occupied_rank == 3); // occupied cells 5 and 10 precede 15
        assert(res.R == 1);             // 3 - 2 = 1
        std::cout << "  • Test 3: Occupied cell tie-breaking and unoccupied exclusion ... [PASS]\n";
    }

    // Performance benchmark
    constexpr size_t ITERS = 100000;
    int64_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto r = evaluate_rank_margin_hinge(
            static_cast<int32_t>(i & 63), 40, 12, 16,
            cell_dist, occupied, static_cast<uint8_t>((i * 2) & 254), 16
        );
        checksum += r.loss_A_num + r.loss_R_num;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == -1) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
