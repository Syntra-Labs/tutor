#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 061 Flock Read Consensus Test Suite ===\n";

    // Test 1: Causal masking and zero-gate identity
    {
        std::vector<FlockNode> ctx = {
            {2, 5, 10}, // eligible (10 < 15)
            {2, 9, 15}, // ineligible (15 >= 15)
            {2, 9, 20}  // ineligible (20 >= 15)
        };
        uint8_t m = 0xFF;
        // Gate = 0 returns v_q unchanged, but still computes consensus m = 5
        uint8_t out_zero = flock_read_step(ctx, 15, 2, 0, 7, 0, &m);
        assert(out_zero == 0);
        assert(m == 5);
        std::cout << "  • Test 1: Causal filter (last_n < query_pos) and gate=0 identity ... [PASS]\n";
    }

    // Test 2: Lexicographic ranking by (d_L1, d_H, -last_n, n) and frame transport
    {
        // Query frame f_q = 0, query_pos = 100, K = 3
        // Node 0: f_n = 0 (d_L1=0, d_H=0), last=10, payload=4 -> transported c = 4^0^0 = 4
        // Node 1: f_n = 0 (d_L1=0, d_H=0), last=50, payload=4 -> transported c = 4
        // Node 2: f_n = 1 (d_L1=4, d_H=1), last=90, payload=5 -> transported c = 5^1^0 = 4
        // Node 3: f_n = 3 (d_L1=8, d_H=2), last=99, payload=15 -> excluded since K=3!
        std::vector<FlockNode> ctx = {
            {0, 4, 10},
            {0, 4, 50},
            {1, 5, 90},
            {3, 15, 99}
        };
        uint8_t m = 0xFF;
        uint8_t out = flock_read_step(ctx, 100, 0, 4, 3, +1, &m);
        assert(m == 4);
        assert(out == 4); // ROOTS[4] + ROOTS[4] snaps back to 4
        std::cout << "  • Test 2: Lexicographic neighbour selection and frame transport ... [PASS]\n";
    }

    // Test 3: Negative gate (-1) subtracts consensus root
    {
        // All 3 neighbours in frame 12 carry payload 12 (all +1s).
        // Query v_q = 15 (all -1s), gate = -1 -> ROOTS[15] - ROOTS[12] = (-2, -2, ..., -2) -> snaps to 15
        // Whereas if v_q = 12 (all +1s) and consensus m = 15 (all -1s) with gate = -1:
        // ROOTS[12] - ROOTS[15] = (+2, +2, ..., +2) -> snaps to 12.
        std::vector<FlockNode> ctx = {
            {12, 15, 1},
            {12, 15, 2},
            {12, 15, 3}
        };
        uint8_t m = 0xFF;
        uint8_t out = flock_read_step(ctx, 10, 12, 12, 3, -1, &m);
        assert(m == 15);
        assert(out == 12);
        std::cout << "  • Test 3: Gated subtraction (gate = -1) verified ... [PASS]\n";
    }

    // Performance benchmark
    std::vector<FlockNode> bench_ctx(64);
    for (size_t i = 0; i < bench_ctx.size(); ++i) {
        bench_ctx[i] = {static_cast<uint8_t>(i & 15), static_cast<uint8_t>((i * 3) & 15), static_cast<uint32_t>(i)};
    }
    constexpr size_t ITERS = 50000;
    uint64_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        uint8_t m = 0;
        checksum += flock_read_step(bench_ctx, 64, static_cast<uint8_t>(i & 15), static_cast<uint8_t>((i >> 2) & 15), 7, 1, &m);
        checksum += m;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0xFFFFFFFFFFFFULL) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
