#pragma once

#include <bit>
#include <cstdint>

// Step a 64-bit periodic ring forward by 1 generation under Wolfram Rule 110:
// l = std::rotl(state, 1), r = std::rotr(state, 1), next = (state ^ r) | (state & ~l)
inline uint64_t step_rule_110(uint64_t state) {
    // TODO: implement 64-bit SWAR Rule 110 step on a periodic ring
    (void)state;
    return 0;
}
