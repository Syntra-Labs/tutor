# 009: Packed 2-Bit Balanced Ternary Representation

## Problem Statement

A balanced ternary vector $w \in \{-1, 0, +1\}^{64}$ is stored in two orthogonal 64-bit bit-planes (`PackedTrit64`):
- `pos`: bit $i$ is `1` iff $w_i = +1$
- `neg`: bit $i$ is `1` iff $w_i = -1$
- Both bits are `0` iff $w_i = 0$.

Implement `PackedTrit64`:
1. `bool is_valid() const`: Verify the disjoint-plane invariant `(pos & neg) == 0`.
2. `PackedTrit64 negate() const`: $O(1)$ negation over all 64 trits by swapping `pos` and `neg`.
3. `size_t nnz() const`: Number of non-zero trits via `std::popcount(pos | neg)`.
4. `PackedTrit64::from_array(const std::array<int8_t, 64>& trits)` and `to_array() const`: Round-trip conversion between `std::array<int8_t, 64>` (values $>0 \mapsto +1$, $<0 \mapsto -1$, $0 \mapsto 0$) and dual bit-planes.

## Mathematical Law / Invariant

- **Orthogonal Plane Exclusivity:** $P \land N = 0$. Because $+1$ and $-1$ are mutually exclusive states, the fourth 2-bit code $(1, 1)$ is forbidden.
- **Involution of Negation:** $\text{negate}(\text{negate}(x)) = x$, and for every coordinate $i$, $\text{to\_array}(\text{negate}(x))_i = -\text{to\_array}(x)_i$.
- **$L_1$ Norm / Support Partition:** Since $P \land N = 0$, $\text{popcount}(P \lor N) = \text{popcount}(P) + \text{popcount}(N) = \sum_{i=0}^{63} |w_i|$.

## Function Signature

```cpp
struct PackedTrit64 {
    uint64_t pos = 0;
    uint64_t neg = 0;

    constexpr bool is_valid() const;
    constexpr PackedTrit64 negate() const;
    constexpr size_t nnz() const;
    static PackedTrit64 from_array(const std::array<int8_t, 64>& trits);
    std::array<int8_t, 64> to_array() const;
};
```

## Complexity Target

- **Time:** $O(1)$ (1 instruction) for `is_valid`, `negate`, and `nnz`; $O(64)$ for array pack/unpack.
- **Space:** 16 bytes (`2 x uint64_t`) for 64 trits (2 bits/trit).
