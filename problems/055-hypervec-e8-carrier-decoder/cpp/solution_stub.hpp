#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

inline constexpr std::array<std::array<int8_t, 8>, 16> CANONICAL_E8_ORBIT_16 = {{
    { 2,  2,  0,  0,  0,  0,  0,  0},
    {-2, -2,  0,  0,  0,  0,  0,  0},
    {-2,  2,  0,  0,  0,  0,  0,  0},
    { 0,  0, -2, -2,  0,  0,  0,  0},
    { 0,  0, -2,  2,  0,  0,  0,  0},
    { 0,  0,  0,  0, -2, -2,  0,  0},
    { 0,  0,  0,  0, -2,  2,  0,  0},
    { 0,  0,  0,  0,  0,  0, -2, -2},
    { 0,  0,  0,  0,  0,  0, -2,  2},
    { 0,  0,  0,  0,  0,  0,  2, -2},
    { 0,  0,  0,  0,  0,  0,  2,  2},
    { 0,  0,  0,  0,  2, -2,  0,  0},
    { 0,  0,  0,  0,  2,  2,  0,  0},
    { 0,  0,  2, -2,  0,  0,  0,  0},
    { 0,  0,  2,  2,  0,  0,  0,  0},
    { 2, -2,  0,  0,  0,  0,  0,  0}
}};

struct Hypervec8192 {
    std::array<uint64_t, 128> words{};
};

struct E8CarrierAddress {
    int32_t first_block[8]{};
    int32_t last_block[8]{};
    std::array<uint8_t, 4> block_roots{};
    uint8_t admission_cell = 0;
};

inline uint8_t snap_e8_root(
    const std::array<std::array<int8_t, 8>, 16>& roots,
    const int32_t x[8],
    uint32_t scale = 1
) {
    uint8_t best_r = 0;
    uint32_t best_dist = 0xFFFFFFFFu;
    uint32_t best_ham = 0xFFFFFFFFu;
    for (uint8_t r = 0; r < 16; ++r) {
        uint32_t dist = 0, ham = 0;
        const auto& rt = roots[r];
        for (size_t d = 0; d < 8; ++d) {
            int32_t y = static_cast<int32_t>(rt[d]) * static_cast<int32_t>(scale);
            int32_t diff = x[d] - y;
            dist += static_cast<uint32_t>(diff < 0 ? -diff : diff);
            ham += static_cast<uint32_t>(x[d] != y);
        }
        if (dist < best_dist || (dist == best_dist && ham < best_ham)) {
            best_r = r;
            best_dist = dist;
            best_ham = ham;
        }
    }
    return best_r;
}

// Decode an 8192-bit hypervector into 4 E8 block roots and a 256-cell admission index
inline E8CarrierAddress decode_e8_carrier(
    const Hypervec8192& hv,
    const std::array<std::array<int8_t, 8>, 16>& roots = CANONICAL_E8_ORBIT_16
) {
    // TODO: partition 128 words into 4 blocks x 8 coords x 4 words, compute (ones - 128)/6,
    // snap each block to a root in [0, 15], and pack admission_cell = (r0 << 4) | r3
    (void)hv; (void)roots;
    return E8CarrierAddress{};
}
