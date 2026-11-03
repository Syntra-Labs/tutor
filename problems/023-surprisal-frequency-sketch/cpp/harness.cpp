#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 023 Online Surprisal Frequency Sketch Suite ===\n";

    SurprisalSketch sketch{};

    // Test 1: Cold-start guard (total_spans <= 4 returns base_weight)
    uint64_t seed_common = 0x123456789ABCDEF0ULL;
    uint64_t seed_rare   = 0xFEDCBA9876543210ULL;
    assert(sketch.weight_for_seed(seed_common, 4) == 4);
    sketch.step_span(3); // total_spans == 4
    assert(sketch.weight_for_seed(seed_common, 4) == 4);
    std::cout << "  • Test 1: Cold-start guard (total_spans <= 4) ... [PASS]\n";

    // Test 2: Stepped IDF weight tiers after advancing to 100 spans
    sketch.step_span(96); // total_spans == 100
    // Unseen seed (df = 0) -> base_weight + 2 = 6
    assert(sketch.weight_for_seed(seed_rare, 4) == 6);

    // Observe seed_common 50 times -> ratio = 101 / 51 = 1 <= 3 -> weight = 1
    sketch.observe_seed(seed_common, 50);
    assert(sketch.weight_for_seed(seed_common, 4) == 1);

    // Observe another seed 15 times -> ratio = 101 / 16 = 6 <= 8 -> weight = 2
    uint64_t seed_mid = 0x0011223344556677ULL;
    sketch.observe_seed(seed_mid, 15);
    assert(sketch.weight_for_seed(seed_mid, 4) == 2);

    // Observe another seed 5 times -> ratio = 101 / 6 = 16 <= 24 -> weight = base_weight (4)
    uint64_t seed_mod = 0x8899AABBCCDDEEFFULL;
    sketch.observe_seed(seed_mod, 5);
    assert(sketch.weight_for_seed(seed_mod, 4) == 4);
    std::cout << "  • Test 2: Stepped IDF salience tiers (1, 2, base, base+2) ... [PASS]\n";

    // Test 3: Incremental un-observation (negative delta) with saturation
    sketch.observe_seed(seed_common, -48); // df drops from 50 to 2 -> ratio = 101 / 3 = 33 > 24 -> weight = 6
    assert(sketch.weight_for_seed(seed_common, 4) == 6);
    sketch.observe_seed(seed_common, -100); // saturates at df = 0
    assert(sketch.weight_for_seed(seed_common, 4) == 6);
    std::cout << "  • Test 3: Reversible negative delta & zero saturation ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 1000000;
    volatile int16_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        sink += sketch.weight_for_seed(seed_common + i, 4);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 023 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
