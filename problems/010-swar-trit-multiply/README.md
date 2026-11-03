# 010: Branchless SWAR Balanced Ternary Multiplication

## Problem Statement

Given two 64-trit vectors `a` and `b` in dual bit-plane format (`PackedTrit64 { uint64_t pos, neg; }`), compute their element-wise product $c_i = a_i \cdot b_i \in \{-1, 0, +1\}$ across all 64 coordinates simultaneously without branches or integer multipliers:
- `out.pos = (a.pos & b.pos) | (a.neg & b.neg)`
- `out.neg = (a.pos & b.neg) | (a.neg & b.pos)`

## Mathematical Law / Invariant

- **Commutative Monoid with Zero Absorption:** $(\{-1, 0, +1\}^{64}, \cdot)$ is a commutative monoid with identity $+1^{64}$ (`{ ~0ULL, 0ULL }`) and absorbing zero $0^{64}$ (`{ 0ULL, 0ULL }`).
- **Sign-Rule Closure:** If `a.is_valid()` and `b.is_valid()`, then `out.is_valid()` holds automatically (`(out.pos & out.neg) == 0`), because $(P_a \land N_a) = 0$ and $(P_b \land N_b) = 0$ imply `out.pos & out.neg == 0`.
- **Involution on Support:** Multiplying `a` by $-1^{64}$ (`{ 0ULL, ~0ULL }`) swaps `a.pos` and `a.neg`, and `trit_mul(a, a)` projects onto the support mask `{ a.pos | a.neg, 0ULL }`.

## Function Signature

```cpp
PackedTrit64 trit_mul(PackedTrit64 a, PackedTrit64 b);
```

## Complexity Target

- **Time:** $O(1)$ — 6 bitwise instructions (`and`, `or`) for 64 parallel trit multiplications (16+ trits/cycle).
- **Space:** $O(1)$ registers.

## Source

- [`ternary-xor.md` (§How it works)](file:///home/yassine/code/uor-r4/docs/mechanisms/ternary-xor.md#L23-L30)
