#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

struct RecordNode {
    uint8_t scale = 0; // 0 = 32-byte leaf atom, >0 = internal ternary node
    uint64_t id = 0;
    uint64_t span_bytes = 32; // total raw capacity covered by this subtree (32 * 3^scale for full subtrees)
    std::array<uint8_t, 32> leaf_bytes{};
    std::array<uint64_t, 3> children{0, 0, 0};
};

struct Manifest {
    size_t byte_len = 0;
    uint64_t root_id = 0;
};

inline uint64_t fnv1a_bytes(const void* data, size_t len, uint64_t h = 0xCBF29CE484222325ULL) {
    const auto* p = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<uint64_t>(p[i]);
        h *= 0x100000001B3ULL;
    }
    return h == 0 ? 1ULL : h;
}

inline uint64_t hash_leaf(const std::array<uint8_t, 32>& leaf) {
    uint8_t tag = 0;
    uint64_t h = fnv1a_bytes(&tag, 1);
    return fnv1a_bytes(leaf.data(), 32, h);
}

inline uint64_t hash_internal(uint8_t scale, uint64_t c0, uint64_t c1, uint64_t c2) {
    uint64_t h = fnv1a_bytes(&scale, 1);
    uint64_t kids[3] = {c0, c1, c2};
    return fnv1a_bytes(kids, sizeof(kids), h);
}

class TernaryObjectStore {
public:
    Manifest fold(const std::vector<uint8_t>& data) {
        // TODO: slice into 32-byte zero-padded atoms and fold into deduplicated ternary Merkle DAG
        (void)data;
        return {};
    }

    std::vector<uint8_t> rebuild(const Manifest& m) const {
        // TODO: reconstruct full byte vector of length m.byte_len
        (void)m;
        return {};
    }

    std::vector<uint8_t> range(
        const Manifest& m,
        size_t start,
        size_t end,
        size_t* out_visited_nodes = nullptr
    ) const {
        // TODO: selectively traverse only subtrees intersecting [start, end)
        (void)m;
        (void)start;
        (void)end;
        if (out_visited_nodes) *out_visited_nodes = 0;
        return {};
    }

    size_t unique_record_count() const {
        return records_.size();
    }

private:
    std::unordered_map<uint64_t, RecordNode> records_;
};
