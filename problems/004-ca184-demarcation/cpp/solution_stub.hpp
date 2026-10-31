#pragma once

#include <cstdint>

// Step a 64-bit register forward using Rule 184
// (left bit flows right into empty spaces)
inline uint64_t step_rule184(uint64_t state, uint64_t left_bit, uint64_t right_bit) {
    // Left neighbor for each bit: (state << 1) | left_bit
    // Right neighbor for each bit: (state >> 1) | (right_bit << 63)
    // TODO: Apply Rule 184 boolean logic: (left & ~current) | (current & right)
    (void)state; (void)left_bit; (void)right_bit;
    return 0;
}
