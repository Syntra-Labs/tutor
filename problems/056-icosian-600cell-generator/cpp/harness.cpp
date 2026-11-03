#include <cassert>
#include <chrono>
#include <iostream>
#include <set>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 056 Binary Icosahedral Group 2I (120 Icosians) Test Suite ===\n";

    auto icosians = generate_icosians_120();

    // Test 1: 120 distinct vertices, each with exact norm 4*(p.p) == 4 + 0*phi
    std::set<Icosian2> unique_set(icosians.begin(), icosians.end());
    assert(unique_set.size() == 120);
    for (const auto& q : icosians) {
        ZPhi n4 = icosian_dot4(q, q);
        assert(n4.a == 4 && n4.b == 0);
    }
    std::cout << "  • Test 1: 120 distinct icosians with exact norm 4*(q.q) == 4 + 0*phi ... [PASS]\n";

    // Test 2: Distance shell distribution (1, 12, 20, 12, 30, 12, 20, 12, 1) from every vertex
    std::array<uint32_t, 9> expected_shells = {1, 12, 20, 12, 30, 12, 20, 12, 1};
    for (size_t idx : {size_t(0), size_t(17), size_t(63), size_t(119)}) {
        auto shells = compute_icosian_shell_counts(icosians, idx);
        assert(shells == expected_shells);
    }
    std::cout << "  • Test 2: Symmetric 9-shell distribution (1,12,20,12,30,12,20,12,1) ... [PASS]\n";

    // Test 3: Total number of 600-cell edges (pairs at shell-1 dot product 4*(p.q) == 2*phi = {0, 2}) is 720
    uint32_t edge_count = 0;
    for (size_t i = 0; i < 120; ++i) {
        for (size_t j = i + 1; j < 120; ++j) {
            if (icosian_dot4(icosians[i], icosians[j]) == ZPhi{0, 2}) {
                ++edge_count;
            }
        }
    }
    assert(edge_count == 720);
    std::cout << "  • Test 3: 600-cell has exactly 720 edges at 36-degree shell (2*phi) ... [PASS]\n";

    constexpr int ITERS = 50000;
    volatile int32_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        ZPhi d = icosian_dot4(icosians[i % 120], icosians[(i * 7) % 120]);
        sink = sink + d.a + d.b;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
