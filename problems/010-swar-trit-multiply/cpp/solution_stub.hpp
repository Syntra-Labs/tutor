#pragma once

#include <cstdint>

struct PackedTrit64 {
    uint64_t pos = 0; // bit i == 1 iff trit[i] == +1
    uint64_t neg = 0; // bit i == 1 iff trit[i] == -1

    constexpr bool is_valid() const { return (pos & neg) == 0; }
};

// Multiply 64 balanced trits in parallel using 6 bitwise operations
constexpr PackedTrit64 trit_mul(PackedTrit64 a, PackedTrit64 b) {
    // TODO: implement out.pos = (a.pos & b.pos) | (a.neg & b.neg)
    //                 out.neg = (a.pos & b.neg) | (a.neg & b.pos)
    (void)a;
    (void)b;
    return {0, 0};
}
