#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

struct ExtractedPatch {
    uint64_t seed;
    int16_t intrinsic_weight;
    bool is_primary_token;
};

struct SparsifiedMassResult {
    std::vector<std::pair<uint64_t, int16_t>> retained_nodes;
    uint64_t primary_mass_seed = 0;
};

inline SparsifiedMassResult sparsify_mass_nodes(
    const ExtractedPatch* patches,
    size_t n_patches,
    size_t max_mass_nodes = 20
) {
    // TODO:
    // 1. Deduplicate patches (up to 192 unique seeds); on duplicate add max<int16_t>(1, w / 2)
    // 2. Find primary_mass_seed (highest weight among seed_is_primary[i] == true, strict >)
    // 3. If num_unique > max_mass_nodes, partial_sort indices by descending
    //    score = weight + (is_primary ? 2 : 0) (tie-breaking by lower index a < b)
    // 4. Return retained (seed, weight) pairs and primary_mass_seed
    (void)patches;
    (void)n_patches;
    (void)max_mass_nodes;
    return {};
}
