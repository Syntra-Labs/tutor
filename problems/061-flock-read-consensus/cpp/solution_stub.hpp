#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

inline constexpr int32_t FLOCK_ROOTS[16][8] = {
    { 2,  2,  0,  0,  0,  0,  0,  0},
    { 2, -2,  0,  0,  0,  0,  0,  0},
    {-2,  2,  0,  0,  0,  0,  0,  0},
    {-2, -2,  0,  0,  0,  0,  0,  0},
    { 0,  0,  2,  2,  0,  0,  0,  0},
    { 0,  0,  2, -2,  0,  0,  0,  0},
    { 0,  0, -2,  2,  0,  0,  0,  0},
    { 0,  0, -2, -2,  0,  0,  0,  0},
    { 0,  0,  0,  0,  2,  2,  0,  0},
    { 0,  0,  0,  0,  2, -2,  0,  0},
    { 0,  0,  0,  0, -2,  2,  0,  0},
    { 0,  0,  0,  0, -2, -2,  0,  0},
    { 1,  1,  1,  1,  1,  1,  1,  1},
    { 1,  1,  1,  1, -1, -1, -1, -1},
    { 1, -1,  1, -1,  1, -1,  1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1}
};

struct FlockNode {
    uint8_t frame_root = 0;   // f_n in [0, 15]
    uint8_t payload_root = 0; // p_n in [0, 15]
    uint32_t last_pos = 0;    // end position of node
};

// Snap an 8D integer vector to the nearest of the 16 FLOCK_ROOTS by L1 distance (ties -> smaller index)
inline uint8_t snap_flock_root(const int32_t vec[8]) {
    uint8_t best_r = 0;
    uint32_t best_l1 = 0xFFFFFFFFU;
    for (uint8_t r = 0; r < 16; ++r) {
        uint32_t l1 = 0;
        for (size_t d = 0; d < 8; ++d) {
            int32_t diff = vec[d] - FLOCK_ROOTS[r][d];
            l1 += static_cast<uint32_t>(diff < 0 ? -diff : diff);
        }
        if (l1 < best_l1) {
            best_l1 = l1;
            best_r = r;
        }
    }
    return best_r;
}

inline uint8_t flock_read_step(
    const std::vector<FlockNode>& context,
    uint32_t query_pos,
    uint8_t f_q,
    uint8_t v_q,
    size_t K,
    int8_t gate_trit,
    uint8_t* out_consensus_root = nullptr
) {
    // TODO: implement integer Flock read ranking, frame transport, consensus snap, and gated update
    (void)context;
    (void)query_pos;
    (void)f_q;
    (void)K;
    (void)gate_trit;
    if (out_consensus_root) *out_consensus_root = 0;
    return v_q;
}
