# 008: CA 184 / CA 30 Counterfactual Match-Inversion Demarcator

## Problem Statement

In grammar-free streaming text segmentation, structural phase boundaries are detected by evolving a 64-bit sliding byte-parity window under three parallel cellular automaton arms:
1. **Forward Conservative Arm (`fwd_1`):** `step_rule_184(seed_bits)`
2. **Counterfactual Inverted Arm (`cf_1`):** `step_rule_184(~seed_bits)`
3. **Chaotic Reference Arm (`r30_1`):** `step_rule_30(seed_bits)`

Implement `count_demarcs(uint64_t seed_bits, bool& particle_conserved_out)`:
- Verify dual particle conservation:
  - `std::popcount(fwd_1) == std::popcount(seed_bits)`
  - `std::popcount(fwd_1) + std::popcount(cf_1) == 64`
- Identify cell indices $i$ where `fwd_1` is the logical inversion of `r30_1` while `cf_1` equals `r30_1`:
  - `matches = ~(fwd_1 ^ ~r30_1) & ~(cf_1 ^ r30_1)`
- Count spatial transitions around the 64-bit ring:
  - `demarcs = matches ^ std::rotl(matches, 1)`
  - Return `std::popcount(demarcs)`.

## Mathematical Law / Invariant

- **Dual Particle Conservation:** Under periodic boundary conditions on $\mathbb{Z}_{64}$, Rule 184 strictly conserves particle count $P(x) = \text{popcount}(x)$ and hole count $P(\neg x) = 64 - P(x)$, guaranteeing:
  $$P(\text{step}_{184}(x)) + P(\text{step}_{184}(\neg x)) = 64 \quad \forall x \in \{0,1\}^{64}$$
- **Even Parity of Ring Demarcations:** On any closed cyclic ring, the number of $0 \leftrightarrow 1$ transitions in `matches ^ rotl(matches, 1)` is always an even integer in $\{0, 2, 4, \dots, 64\}$.

## Function Signature

```cpp
uint64_t step_rule_184(uint64_t state);
uint64_t step_rule_30(uint64_t state);
uint32_t count_demarcs(uint64_t seed_bits, bool& particle_conserved_out);
```

## Complexity Target

- **Time:** $O(1)$ (~18 branchless bitwise instructions + 4 `popcnt`).
- **Space:** $O(1)$ registers.

## Source

- [`manifold.cpp` (lines 767–809)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L767-L809)
