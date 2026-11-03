#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>

struct PackedTrit64 {
    uint64_t pos = 0; // bit i == 1 iff trit[i] == +1
    uint64_t neg = 0; // bit i == 1 iff trit[i] == -1

    constexpr bool is_valid() const { return (pos & neg) == 0; }
    constexpr PackedTrit64 negate() const { return {neg, pos}; }
};

// Multiply 64 balanced trits in parallel
constexpr PackedTrit64 trit_mul(PackedTrit64 a, PackedTrit64 b) {
    // TODO: implement SWAR balanced ternary multiplication
    (void)a;
    (void)b;
    return {0, 0};
}

// Compute exact multiplier-free inner product over spans of PackedTrit64
inline int64_t trit_dot(std::span<const PackedTrit64> a, std::span<const PackedTrit64> b) {
    // TODO: accumulate std::popcount(p.pos) - std::popcount(p.neg) across words
    (void)a;
    (void)b;
    return 0;
}
