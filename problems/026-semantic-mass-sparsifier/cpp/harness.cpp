#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 026 Semantic Mass Node 90% Sparsifier Suite ===\n";

    // Test 1: Deduplication accumulates max(1, w / 2) and selects primary_mass_seed
    ExtractedPatch p1[] = {
        {100ULL, 4, true},
        {200ULL, 6, false}, // non-primary, even though weight is 6, shouldn't be primary_mass_seed
        {100ULL, 4, true},  // duplicate of 100ULL -> weight becomes 4 + max(1, 4/2) = 6
        {300ULL, 5, true}
    };
    auto r1 = sparsify_mass_nodes(p1, 4, 20);
    assert(r1.retained_nodes.size() == 3);
    assert(r1.primary_mass_seed == 100ULL);
    assert(r1.retained_nodes[0].first == 100ULL && r1.retained_nodes[0].second == 6);
    std::cout << "  • Test 1: Duplicate weight reinforcement & primary_mass_seed selection ... [PASS]\n";

    // Test 2: Top-K truncation via score = weight + (is_primary ? 2 : 0)
    std::vector<ExtractedPatch> many;
    for (uint64_t i = 1; i <= 30; ++i) {
        many.push_back({i, static_cast<int16_t>(i), (i % 2 == 0)});
    }
    auto r2 = sparsify_mass_nodes(many.data(), many.size(), 5);
    assert(r2.retained_nodes.size() == 5);
    assert(r2.primary_mass_seed == 30ULL);
    // Top scores:
    // i=30 (primary): 30+2=32
    // i=28 (primary): 28+2=30
    // i=29 (non-prim): 29+0=29
    // i=26 (primary): 26+2=28
    // i=27 (non-prim): 27+0=27
    assert(r2.retained_nodes[0].first == 30ULL);
    assert(r2.retained_nodes[1].first == 28ULL);
    assert(r2.retained_nodes[2].first == 29ULL);
    assert(r2.retained_nodes[3].first == 26ULL);
    assert(r2.retained_nodes[4].first == 27ULL);
    std::cout << "  • Test 2: Top-K salience partial_sort ordering with primary bonus ... [PASS]\n";

    // Test 3: Empty input safety
    auto r3 = sparsify_mass_nodes(nullptr, 0, 20);
    assert(r3.retained_nodes.empty());
    assert(r3.primary_mass_seed == 0ULL);
    std::cout << "  • Test 3: Empty input handled cleanly ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 100000;
    volatile uint64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto res = sparsify_mass_nodes(many.data(), many.size(), 10);
        sink ^= res.primary_mass_seed;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 026 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
