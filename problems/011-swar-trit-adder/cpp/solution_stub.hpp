#pragma once

#include <cstdint>

struct PackedTrit64 {
    uint64_t pos = 0; // bit i == 1 iff trit[i] == +1
    uint64_t neg = 0; // bit i == 1 iff trit[i] == -1

    constexpr bool is_valid() const { return (pos & neg) == 0; }
    constexpr PackedTrit64 negate() const { return {neg, pos}; }
};

struct TritAddResult {
    PackedTrit64 sum;
    PackedTrit64 carry;
};

// 64-lane SWAR balanced ternary half-adder: a + b = 3 * carry + sum
constexpr TritAddResult trit_half_add(PackedTrit64 a, PackedTrit64 b) {
    // TODO: implement branchless SWAR balanced ternary half-adder
    (void)a;
    (void)b;
    return {{0, 0}, {0, 0}};
}

// 64-lane SWAR balanced ternary full-adder: a + b + carry_in = 3 * carry + sum
constexpr TritAddResult trit_full_add(PackedTrit64 a, PackedTrit64 b, PackedTrit64 carry_in) {
    // TODO: implement branchless SWAR balanced ternary full-adder
    (void)a;
    (void)b;
    (void)carry_in;
    return {{0, 0}, {0, 0}};
}
