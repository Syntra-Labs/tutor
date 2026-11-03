#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <set>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 053 16-Root E8 Orbit & Transport BFS Test Suite ===\n";

    E8Orbit16 orb = build_e8_orbit_16();

    // Test 1: Anchor root at index 0 and 16 distinct roots with L1=4, L2^2=8
    std::array<int8_t, 8> expected_anchor = {2, 2, 0, 0, 0, 0, 0, 0};
    assert(orb.roots[0] == expected_anchor);
    std::set<std::array<int8_t, 8>> unique_roots(orb.roots.begin(), orb.roots.end());
    assert(unique_roots.size() == 16);
    for (const auto& rt : orb.roots) {
        int l1 = 0, l2_sq = 0;
        for (int8_t v : rt) {
            l1 += std::abs(static_cast<int>(v));
            l2_sq += v * v;
        }
        assert(l1 == 4 && l2_sq == 8);
    }
    std::cout << "  • Test 1: 16 distinct orbit roots with ||r||_1 = 4, ||r||_2^2 = 8 ... [PASS]\n";

    // Test 2: Diagonal of bfs[r][r] is identity (0)
    for (size_t r = 0; r < 16; ++r) {
        assert(orb.bfs[r][r] == 0);
    }
    std::cout << "  • Test 2: Reflexive transport bfs[r][r] == identity ... [PASS]\n";

    // Test 3: Exact transitive transport invariant action(bfs[src][dst], roots[src]) == roots[dst]
    for (size_t src = 0; src < 16; ++src) {
        int32_t x[8], y[8];
        for (size_t d = 0; d < 8; ++d) x[d] = orb.roots[src][d];
        for (size_t dst = 0; dst < 16; ++dst) {
            uint8_t op = orb.bfs[src][dst];
            orb.action(op, x, y);
            for (size_t d = 0; d < 8; ++d) {
                assert(y[d] == static_cast<int32_t>(orb.roots[dst][d]));
            }
        }
    }
    std::cout << "  • Test 3: All 256 pairs satisfy action(bfs[src][dst], roots[src]) == roots[dst] ... [PASS]\n";

    constexpr int ITERS = 200000;
    int32_t x[8] = {2, 2, 0, 0, 0, 0, 0, 0};
    int32_t y[8]{};
    volatile int32_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        uint8_t op = orb.bfs[i & 15][(i >> 4) & 15];
        orb.action(op, x, y);
        sink = sink + y[0];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
