#include <cassert>
#include <chrono>
#include <iostream>
#include <string_view>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 025 Prime-Stride Skip-Gram & CamelCase Extractor Suite ===\n";

    // Test 1: Non-commutativity of bigram and skipgram prime-stride binding
    uint64_t sA = fnv1a_64("alpha", 5);
    uint64_t sB = fnv1a_64("beta", 4);
    assert(make_bigram_seed(sA, sB) != 0);
    assert(make_bigram_seed(sA, sB) != make_bigram_seed(sB, sA));
    assert(make_skipgram_seed(sA, sB) != make_skipgram_seed(sB, sA));
    assert(make_bigram_seed(sA, sB) != make_skipgram_seed(sA, sB));
    std::cout << "  • Test 1: Non-commutative bigram & skip-gram prime-stride binding ... [PASS]\n";

    // Test 2: CamelCase splitting ("hyperVec") -> "hyper" (primary, w=4, plus 4-gram "hype" w=2, plus 3-prefix "hyp" w=1)
    // and "vec" (primary, w=3)
    ExtractedPatch patches[128]{};
    size_t n = extract_patches("hyperVec", patches);
    assert(n == 4);
    assert(patches[0].seed == fnv1a_64("hyper", 5));
    assert(patches[0].intrinsic_weight == 4 && patches[0].is_primary_token);
    assert(patches[1].seed == fnv1a_64("hype", 4, 0x9E3779B97F4A7C15ULL));
    assert(patches[1].intrinsic_weight == 2 && !patches[1].is_primary_token);
    assert(patches[2].seed == fnv1a_64("hyp", 3, 0xBF58476D1CE4E5B9ULL));
    assert(patches[2].intrinsic_weight == 1 && !patches[2].is_primary_token);
    assert(patches[3].seed == fnv1a_64("vec", 3));
    assert(patches[3].intrinsic_weight == 3 && patches[3].is_primary_token);
    std::cout << "  • Test 2: CamelCase decomposition, 4-gram & 3-prefix extraction ... [PASS]\n";

    // Test 3: Short 2-char token weight == 2 and 1-char token ignored
    size_t n2 = extract_patches("a fn xy", patches);
    assert(n2 == 2);
    assert(patches[0].seed == fnv1a_64("fn", 2) && patches[0].intrinsic_weight == 2 && patches[0].is_primary_token);
    assert(patches[1].seed == fnv1a_64("xy", 2) && patches[1].intrinsic_weight == 2 && patches[1].is_primary_token);
    std::cout << "  • Test 3: Short token weights and single-char filtering ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 100000;
    std::string_view bench_code = "inline Hypervec8192 computeHelixSignature(const SurprisalSketch& sketchNode)";
    volatile size_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        sink += extract_patches(bench_code, patches);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 025 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
