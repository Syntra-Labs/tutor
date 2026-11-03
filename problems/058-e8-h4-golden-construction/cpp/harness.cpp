#include <cassert>
#include <chrono>
#include <iostream>
#include <set>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 058 E8 = H4 + phi*H4 Golden Construction Test Suite ===\n";

    auto icosians = generate_icosians_120();
    auto roots = build_e8_golden_roots_240(icosians);

    // Test 1: 240 distinct roots, every root has reduced norm <r, r> == 2
    std::set<Icosian2> root_set(roots.begin(), roots.end());
    assert(root_set.size() == 240);
    for (const auto& r : roots) {
        assert(e8_reduced_inner_product(r, r) == 2);
    }
    std::cout << "  • Test 1: 240 distinct roots in 2I U phi*2I with <r, r> == 2 ... [PASS]\n";

    // Test 2: Exact E8 inner-product distribution (1, 56, 126, 56, 1) from both H4 and phi*H4 roots
    std::array<uint32_t, 5> expected_dist = {1, 56, 126, 56, 1};
    for (size_t idx : {size_t(0), size_t(42), size_t(120), size_t(199), size_t(239)}) {
        assert(e8_inner_product_distribution(roots, idx) == expected_dist);
    }
    std::cout << "  • Test 2: E8 inner-product counts (1, 56, 126, 56, 1) across both shells ... [PASS]\n";

    // Test 3: Weyl reflection closure: x - <x, r>*r is always in the 240-root set
    for (size_t i = 0; i < 240; i += 19) {
        for (size_t j = 0; j < 240; j += 23) {
            Icosian2 refl = e8_weyl_reflect(roots[i], roots[j]);
            assert(root_set.count(refl) == 1);
        }
    }
    std::cout << "  • Test 3: Weyl reflection closure across 2I U phi*2I ... [PASS]\n";

    constexpr int ITERS = 50000;
    volatile int32_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        sink = sink + e8_reduced_inner_product(roots[i % 240], roots[(i * 13) % 240]);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
