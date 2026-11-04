# 012: Multiplier-Free Ternary Dot Product

## Problem Statement

For two balanced ternary vectors $a, b \in \{-1, 0, +1\}^{64N}$ represented as spans of `PackedTrit64`, their exact inner product requires zero integer multiplications:
$$\langle a, b \rangle = \sum_{w=0}^{N-1} \Big( \operatorname{popcount}(P_{a,w}\land P_{b,w} \;\lor\; N_{a,w}\land N_{b,w}) - \operatorname{popcount}(P_{a,w}\land N_{b,w} \;\lor\; N_{a,w}\land P_{b,w}) \Big)$$

Implement `int64_t trit_dot(std::span<const PackedTrit64> a, std::span<const PackedTrit64> b)` using `trit_mul` and `std::popcount`.

## Mathematical Law / Invariant

- **Exact Bilinear Inner Product:** Each coordinate $i$ contributes $+1$ iff $a_i = b_i \neq 0$, $-1$ iff $a_i = -b_i \neq 0$, and $0$ if either coordinate is $0$.
- **Self-Inner-Product ($L_2^2 = L_1$ Support):** Since $(-1)^2 = (+1)^2 = 1$ and $0^2 = 0$, $\langle a, a \rangle = \sum_i a_i^2 = \sum_i |a_i| = \operatorname{nnz}(a) \ge 0$.
- **Bilinear Skew under Negation:** $\langle a, -b \rangle = -\langle a, b \rangle$.

## Function Signature

```cpp
struct PackedTrit64 {
    uint64_t pos = 0;
    uint64_t neg = 0;
    constexpr bool is_valid() const { return (pos & neg) == 0; }
    constexpr PackedTrit64 negate() const { return {neg, pos}; }
};

constexpr PackedTrit64 trit_mul(PackedTrit64 a, PackedTrit64 b);
int64_t trit_dot(std::span<const PackedTrit64> a, std::span<const PackedTrit64> b);
```

## Complexity Target

- **Time:** $O(N)$ words — 6 bitwise ops + 2 `popcnt` instructions per 64-trit word.
- **Space:** $O(1)$ registers.
