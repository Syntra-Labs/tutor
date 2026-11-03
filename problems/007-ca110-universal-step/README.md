# 007: Cellular Automaton Rule 110 Step (64-Bit SWAR)

## Problem Statement

Elementary Cellular Automaton **Rule 110** sits at the Class-4 "edge of chaos" and was proven by Matthew Cook (2004) to be Turing-complete via interacting gliders over a periodic "ether" background.

Implement a 64-bit SWAR periodic ring step for Rule 110:
- Left neighbor register: `l = std::rotl(state, 1)`
- Right neighbor register: `r = std::rotr(state, 1)`
- Parallel 64-cell update: `(state ^ r) | (state & ~l)`

## Mathematical Law / Invariant

- **Local Transition Law:** For each bit $i \in \mathbb{Z}_{64}$:
  $$x_i^{t+1} = (c_i^t \oplus r_i^t) \lor (c_i^t \land \neg l_i^t)$$
  Truth table over $(l, c, r) \in \{111, 110, 101, 100, 011, 010, 001, 000\}$: outputs $01101110_2 = 110_{10}$.
- **Cyclic Shift Equivariance:**
  $$\text{step\_rule\_110}(\text{rotl}(x, k)) = \text{rotl}(\text{step\_rule\_110}(x), k)$$
- **Quiescent & Saturated Fixed Points:** Both `0x0000000000000000ULL` and `0xFFFFFFFFFFFFFFFFULL` map to `0x0000000000000000ULL`.

## Function Signature

```cpp
uint64_t step_rule_110(uint64_t state);
```

## Complexity Target

- **Time:** $O(1)$ — 5 CPU instructions updating all 64 cells in parallel.
- **Space:** $O(1)$ registers.

## Source

- [`manifold.cpp` (lines 775–780)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L775-L780)
