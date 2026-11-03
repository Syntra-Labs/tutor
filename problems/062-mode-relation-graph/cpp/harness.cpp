#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 062 MODE_RELATION Graph Coupler Test Suite ===\n";

    std::vector<BlockMeta> cells = {
        {0x123456789ABCDEF0ULL, 42, {~0ULL, ~0ULL, 0ULL, 0ULL}},
        {0x9999999999999999ULL, 10, {~0ULL, 0ULL, ~0ULL, 0ULL}},
        {0x123456789ABCDEF0ULL, 42, {0ULL, ~0ULL, ~0ULL, 0ULL}}
    };
    std::vector<ModeRelationLinks> rel_links;
    std::vector<HelixSignature256> triad_sigs;
    RelationIndexerState state;

    uint32_t prev_seq = ABSENT_NODE;
    for (uint32_t i = 0; i < 3; ++i) {
        wire_block_relations(i, prev_seq, cells, rel_links, triad_sigs, state);
    }

    // Test 1: Verify all 5 edges wired properly across the 3 blocks
    {
        assert(rel_links.size() == 3);
        assert(triad_sigs.size() == 1);
        // Bitwise majority of (~0,~0,0,0), (~0,0,~0,0), (0,~0,~0,0) is (~0,~0,~0,0)
        assert(triad_sigs[0].s0 == ~0ULL);
        assert(triad_sigs[0].s1 == ~0ULL);
        assert(triad_sigs[0].s2 == ~0ULL);
        assert(triad_sigs[0].s3 == 0ULL);

        // e=0: triad_sig_idx
        assert(rel_links[0].triad_sig_idx == 0);
        assert(rel_links[1].triad_sig_idx == 0);
        assert(rel_links[2].triad_sig_idx == 0);

        // e=1: triad siblings
        assert(rel_links[0].triad_sib0_idx == 1);
        assert(rel_links[1].triad_sib0_idx == 0);
        assert(rel_links[1].triad_sib1_idx == 2);
        assert(rel_links[2].triad_sib0_idx == 1);

        // e=2: sequential prev/next
        assert(rel_links[0].seq_next_idx == 1);
        assert(rel_links[1].seq_prev_idx == 0 && rel_links[1].seq_next_idx == 2);
        assert(rel_links[2].seq_prev_idx == 1);

        // e=3 & e=4: block 2 shares mass_seed and sector_id with block 0
        assert(rel_links[2].mass_twin_idx == 0);
        assert(rel_links[2].frame_twin_idx == 0);
        std::cout << "  • Test 1: All 5 structural edge types (e=0..4) wired ... [PASS]\n";
    }

    // Test 2: High resonance query triggers capped bonus (+0.025)
    {
        HelixSignature256 q_sig{~0ULL, ~0ULL, ~0ULL, 0ULL};
        double sim = compute_coupled_similarity(2, 0.60, 0.56, q_sig, cells, rel_links, triad_sigs);
        double expected = 0.85 * 0.60 + 0.15 * 0.56 + 0.025;
        assert(std::abs(sim - expected) < 1e-9);
        std::cout << "  • Test 2: Maximum structural bonus cap (+0.025) verified ... [PASS]\n";
    }

    // Test 3: Orthogonal/anti-correlated query triggers zero bonus
    {
        HelixSignature256 anti_sig{0ULL, 0ULL, 0ULL, ~0ULL};
        double sim = compute_coupled_similarity(2, 0.50, 0.50, anti_sig, cells, rel_links, triad_sigs);
        double expected = 0.85 * 0.50 + 0.15 * 0.50;
        assert(std::abs(sim - expected) < 1e-9);
        std::cout << "  • Test 3: Sub-threshold neighbor resonance adds 0.0 bonus ... [PASS]\n";
    }

    // Performance benchmark
    constexpr size_t ITERS = 100000;
    HelixSignature256 q_sig{~0ULL, ~0ULL, ~0ULL, 0ULL};
    double acc = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        q_sig.s0 ^= i;
        acc += compute_coupled_similarity(i % 3, 0.58, 0.54, q_sig, cells, rel_links, triad_sigs);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (acc == -1.0) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
