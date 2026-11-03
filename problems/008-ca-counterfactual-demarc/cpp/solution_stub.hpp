#pragma once

#include <bit>
#include <cstdint>

// Rule 184 on 64-bit periodic ring: (l & ~c) | (c & r)
inline uint64_t step_rule_184(uint64_t state) {
    // TODO: implement 64-bit SWAR Rule 184 step
    (void)state;
    return 0;
}

// Rule 30 on 64-bit periodic ring: l ^ (c | r)
inline uint64_t step_rule_30(uint64_t state) {
    // TODO: implement 64-bit SWAR Rule 30 step
    (void)state;
    return 0;
}

// Counterfactual match-inversion demarcation detector
inline uint32_t count_demarcs(uint64_t seed_bits, bool& particle_conserved_out) {
    // TODO: evolve fwd_1 = step_rule_184(seed_bits), cf_1 = step_rule_184(~seed_bits),
    // r30_1 = step_rule_30(seed_bits), verify conservation, and return popcount(matches ^ rotl(matches, 1))
    (void)seed_bits;
    particle_conserved_out = false;
    return 0;
}
