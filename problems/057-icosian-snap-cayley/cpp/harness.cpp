#include <cassert>
#include <chrono>
#include <iostream>
#include <set>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 057 600-Cell Voronoi Snapper & 120x120 Cayley Table Test Suite ===\n";

    auto icosians = generate_icosians_120();
    auto cayley = build_icosian_cayley_table(icosians);

    // Test 1: Group closure & Latin square property (every row and column is a permutation of 0..119)
    for (size_t i = 0; i < 120; ++i) {
        std::set<uint8_t> row_set, col_set;
        for (size_t j = 0; j < 120; ++j) {
            assert(cayley[i][j] < 120);
            row_set.insert(cayley[i][j]);
            col_set.insert(cayley[j][i]);
        }
        assert(row_set.size() == 120 && col_set.size() == 120);
    }
    std::cout << "  • Test 1: 120x120 Cayley table closure & Latin square property ... [PASS]\n";

    // Test 2: Associativity and non-commutativity of 2I
    size_t non_comm_pairs = 0;
    for (size_t i = 0; i < 120; ++i) {
        for (size_t j = 0; j < 120; ++j) {
            if (cayley[i][j] != cayley[j][i]) ++non_comm_pairs;
        }
    }
    assert(non_comm_pairs > 0);
    for (size_t i = 0; i < 120; i += 11) {
        for (size_t j = 0; j < 120; j += 13) {
            for (size_t k = 0; k < 120; k += 17) {
                assert(cayley[cayley[i][j]][k] == cayley[i][cayley[j][k]]);
            }
        }
    }
    std::cout << "  • Test 2: Associativity and non-commutativity verified ... [PASS]\n";

    // Test 3: Voronoi snapping idempotence on all 120 vertices (with small perturbation)
    for (uint8_t idx = 0; idx < 120; ++idx) {
        double w = 0.5 * icosians[idx][0].to_double() + 1e-4;
        double x = 0.5 * icosians[idx][1].to_double() - 1e-4;
        double y = 0.5 * icosians[idx][2].to_double() + 0.5e-4;
        double z = 0.5 * icosians[idx][3].to_double() - 0.5e-4;
        assert(snap_icosian(icosians, w, x, y, z) == idx);
    }
    std::cout << "  • Test 3: Voronoi snapping recovers all 120 vertices ... [PASS]\n";

    constexpr int ITERS = 50000;
    volatile uint8_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        sink = sink + snap_icosian(icosians, 0.8, -0.3 + (i & 7) * 0.01, 0.4, 0.2);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
