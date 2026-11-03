#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

struct PackedTrit64 {
    uint64_t pos = 0; // bit i == 1 iff trit[i] == +1
    uint64_t neg = 0; // bit i == 1 iff trit[i] == -1

    // Disjoint plane invariant: no position can be simultaneously +1 and -1
    constexpr bool is_valid() const {
        // TODO: implement (pos & neg) == 0
        return false;
    }

    // O(1) balanced ternary negation (-x swaps positive and negative planes)
    constexpr PackedTrit64 negate() const {
        // TODO: return negated PackedTrit64
        return {0, 0};
    }

    // Count of non-zero trits (L1 norm)
    constexpr size_t nnz() const {
        // TODO: return popcount of active trits
        return 0;
    }

    // Pack 64 signed trits in {-1, 0, +1} into dual bit-planes
    static inline PackedTrit64 from_array(const std::array<int8_t, 64>& trits) {
        // TODO: pack >0 into pos bit i, <0 into neg bit i
        (void)trits;
        return {0, 0};
    }

    // Unpack dual bit-planes into 64 signed trits in {-1, 0, +1}
    inline std::array<int8_t, 64> to_array() const {
        // TODO: unpack pos/neg bits into int8_t {-1, 0, +1}
        std::array<int8_t, 64> out{};
        return out;
    }
};
