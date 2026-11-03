#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

struct HelixSignature256 {
    uint64_t s0 = 0;
    uint64_t s1 = 0;
    uint64_t s2 = 0;
    uint64_t s3 = 0;
};

struct HaloHeader {
    uint8_t sector_id = 0;
    bool rejected = false;
};

struct HotRouteEntry {
    HaloHeader halo{};
    HelixSignature256 sig{};
};

struct ShortlistCand {
    uint32_t cell_idx = 0;
    uint16_t coarse_cost = 0;
};

inline std::vector<ShortlistCand> run_helix_admission_cascade(
    const HelixSignature256& q_sig,
    const std::array<bool, 256>& is_admitted_cell,
    const std::vector<HotRouteEntry>& hot_route,
    size_t max_verify = 144,
    uint16_t cutoff_cost = 212
) {
    // TODO: implement two-stage HelixSignature256 + E8 admission shortlist filter
    (void)q_sig;
    (void)is_admitted_cell;
    (void)hot_route;
    (void)max_verify;
    (void)cutoff_cost;
    return {};
}
