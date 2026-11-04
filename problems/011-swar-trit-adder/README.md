# 011: Branchless SWAR Balanced Ternary Full-Adder

## Problem Statement

In balanced ternary arithmetic over $\{-1, 0, +1\}$, adding three trits $a_i, b_i, c_i \in \{-1, 0, +1\}$ yields a total in $[-3, +3]$, which decomposes uniquely into a balanced ternary carry and sum:
$$a_i + b_i + c_i = 3 \cdot \text{carry}_i + \text{sum}_i, \quad \text{carry}_i, \text{sum}_i \in \{-1, 0, +1\}$$

For example, $+1 + +1 = +2 = 3(+1) + (-1)$, so $(\text{sum}, \text{carry}) = (-1, +1)$.

Implement `trit_half_add` and `trit_full_add` operating in parallel across all 64 trit lanes of `PackedTrit64`:
1. `TritAddResult trit_half_add(PackedTrit64 a, PackedTrit64 b)`:
   - `carry.pos = a.pos & b.pos`
   - `carry.neg = a.neg & b.neg`
   - `sum.pos = ((a.pos ^ b.pos) & ~(a.neg | b.neg)) | carry.neg`
   - `sum.neg = ((a.neg ^ b.neg) & ~(a.pos | b.pos)) | carry.pos`
2. `TritAddResult trit_full_add(PackedTrit64 a, PackedTrit64 b, PackedTrit64 carry_in)`:
   - Chain two half-adders `(s1, c1) = trit_half_add(a, b)` and `(sum, c2) = trit_half_add(s1, carry_in)`, then combine `carry = trit_half_add(c1, c2).sum`.

## Mathematical Law / Invariant

- **Exact Decomposition Law:** For every lane $i \in [0, 63]$:
  $$a_i + b_i + \text{carry\_in}_i = 3 \cdot \text{carry}_i + \text{sum}_i$$
- **Carry Bound Closure:** Because $|a_i + b_i + \text{carry\_in}_i| \le 3$, the intermediate carries $c_1, c_2$ never both equal $+1$ or both equal $-1$; their sum $c_1 + c_2$ always lies strictly in $\{-1, 0, +1\}$.
- **Negation Anti-Homomorphism:** $\text{trit\_full\_add}(-a, -b, -c) = (-\text{sum}, -\text{carry})$.

## Function Signature

```cpp
struct PackedTrit64 {
    uint64_t pos = 0;
    uint64_t neg = 0;
    constexpr bool is_valid() const { return (pos & neg) == 0; }
};

struct TritAddResult {
    PackedTrit64 sum;
    PackedTrit64 carry;
};

constexpr TritAddResult trit_half_add(PackedTrit64 a, PackedTrit64 b);
constexpr TritAddResult trit_full_add(PackedTrit64 a, PackedTrit64 b, PackedTrit64 carry_in);
```

## Complexity Target

- **Time:** $O(1)$ (~18 branchless bitwise operations for 64 parallel full-adder lanes).
- **Space:** $O(1)$ registers.
