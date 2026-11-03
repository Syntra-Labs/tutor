#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

inline constexpr uint32_t ABSENT_NODE = 0xFFFFFFFFU;

struct HelixSignature256 {
    uint64_t s0 = 0;
    uint64_t s1 = 0;
    uint64_t s2 = 0;
    uint64_t s3 = 0;
};

struct BlockMeta {
    uint64_t primary_mass_seed = 0;
    uint8_t sector_id = 0;
    HelixSignature256 sig{};
};

struct ModeRelationLinks {
    uint32_t triad_sig_idx  = ABSENT_NODE; // e=0: Folded 3-block Triad parent signature
    uint32_t triad_sib0_idx = ABSENT_NODE; // e=1: Triad sibling A
    uint32_t triad_sib1_idx = ABSENT_NODE; // e=1: Triad sibling B
    uint32_t seq_prev_idx   = ABSENT_NODE; // e=2: Sequential previous block in file
    uint32_t seq_next_idx   = ABSENT_NODE; // e=2: Sequential next block in file
    uint32_t mass_twin_idx  = ABSENT_NODE; // e=3: Block sharing identical primary mass seed
    uint32_t frame_twin_idx = ABSENT_NODE; // e=4: Block in identical E8 admission sector
    uint64_t primary_mass_seed = 0;
};

struct RelationIndexerState {
    std::array<uint32_t, 1024> online_mass_table;
    std::array<uint32_t, 256> online_frame_table;
    uint32_t online_triad_buf[3]{};
    size_t online_triad_count = 0;

    RelationIndexerState() {
        online_mass_table.fill(ABSENT_NODE);
        online_frame_table.fill(ABSENT_NODE);
    }
};

inline double helix_sig_similarity(const HelixSignature256& a, const HelixSignature256& b) {
    uint32_t ham = static_cast<uint32_t>(
        std::popcount(a.s0 ^ b.s0) +
        std::popcount(a.s1 ^ b.s1) +
        std::popcount(a.s2 ^ b.s2) +
        std::popcount(a.s3 ^ b.s3)
    );
    return 1.0 - static_cast<double>(ham) / 256.0;
}

inline void wire_block_relations(
    uint32_t cell_idx,
    uint32_t& prev_seq_in_file,
    const std::vector<BlockMeta>& cells,
    std::vector<ModeRelationLinks>& rel_links,
    std::vector<HelixSignature256>& triad_sigs,
    RelationIndexerState& state
) {
    // TODO: implement O(1) 5-edge structural wiring (e=0..4)
    (void)cell_idx;
    (void)prev_seq_in_file;
    (void)cells;
    (void)rel_links;
    (void)triad_sigs;
    (void)state;
}

inline double compute_coupled_similarity(
    uint32_t cell_idx,
    double raw_sim,
    double file_sim,
    const HelixSignature256& q_sig,
    const std::vector<BlockMeta>& cells,
    const std::vector<ModeRelationLinks>& rel_links,
    const std::vector<HelixSignature256>& triad_sigs
) {
    // TODO: implement 5-edge MODE_RELATION coupled similarity evaluation
    (void)cell_idx;
    (void)raw_sim;
    (void)file_sim;
    (void)q_sig;
    (void)cells;
    (void)rel_links;
    (void)triad_sigs;
    return 0.0;
}
