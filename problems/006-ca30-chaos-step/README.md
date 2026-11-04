# 006: Cellular Automaton Rule 30 Step (64-Bit SWAR)

## Problem Statement

Stephen Wolfram's elementary Cellular Automaton **Rule 30** exhibits Class-3 aperiodic chaos from single-bit initial conditions and serves as the chaotic reference arm in counterfactual stream demarcation.

Implement a 64-bit SWAR (SIMD Within A Register) periodic ring step for Rule 30:
- Left neighbor register: `l = std::rotl(state, 1)`
- Right neighbor register: `r = std::rotr(state, 1)`
- Parallel 64-cell update: `l ^ (state | r)`

## Mathematical Law / Invariant

- **Local Transition Law:** For cell $i$ on a 64-bit cyclic ring ($\mathbb{Z}_{64}$):
  $$x_i^{t+1} = l_i^t \oplus (c_i^t \lor r_i^t)$$
  Truth table over $(l, c, r) \in \{111, 110, 101, 100, 011, 010, 001, 000\}$: outputs $00011110_2 = 30_{10}$.
- **Cyclic Shift Equivariance:** For any shift $k \in [0, 63]$:
  $$\text{step\_rule\_30}(\text{rotl}(x, k)) = \text{rotl}(\text{step\_rule\_30}(x), k)$$
- **Non-Conservative Chaos:** Unlike Rule 184, Rule 30 does not conserve `std::popcount(state)`.

## Function Signature

```cpp
uint64_t step_rule_30(uint64_t state);
```

## Complexity Target

- **Time:** $O(1)$ — 4 CPU instructions (`rol`, `ror`, `or`, `xor`) updating all 64 cells simultaneously.
- **Space:** $O(1)$ registers.
