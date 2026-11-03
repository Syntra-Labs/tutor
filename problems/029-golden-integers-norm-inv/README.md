# 029: $\mathbb{Z}[\phi]$ Galois Conjugate, Norm & Unit Inversion

## Problem Statement

The polynomial $x^2 - x - 1 = 0$ has two roots: $\phi = \frac{1+\sqrt{5}}{2}$ and $\bar{\phi} = 1 - \phi = -\phi^{-1} = \frac{1-\sqrt{5}}{2}$. Swapping $\phi \mapsto 1 - \phi$ defines the non-trivial Galois automorphism (conjugation) of the quadratic field $\mathbb{Q}(\sqrt{5})$ restricted to $\mathbb{Z}[\phi]$.

Implement `conj()`, `norm()`, and exact unit inversion `inv()` on `ZPhi`.

## Mathematical Law / Invariant

1. **Galois Conjugate:**
   $$(a + b\phi)^\star = a + b(1 - \phi) = (a + b) - b\phi \implies \text{conj}(a, b) = (a + b, -b)$$
   Involution & ring homomorphism: $(x^\star)^\star = x$, $(x + y)^\star = x^\star + y^\star$, and $(x \cdot y)^\star = x^\star \cdot y^\star$.
2. **Algebraic Norm:**
   $$N(a + b\phi) = (a + b\phi)(a + b\phi)^\star = a^2 + ab - b^2 \in \mathbb{Z}$$
   **Multiplicativity Law:** $N(x \cdot y) = N(x) \cdot N(y)$ for all $x, y \in \mathbb{Z}[\phi]$.
3. **Unit Group Characterization ($\mathbb{Z}[\phi]^\times$):**
   An element $x \in \mathbb{Z}[\phi]$ has a multiplicative inverse in $\mathbb{Z}[\phi]$ if and only if $N(x) = \pm 1$:
   $$x^{-1} = \frac{x^\star}{N(x)} = \begin{cases} x^\star & \text{if } N(x) = +1 \\ -x^\star & \text{if } N(x) = -1 \\ \text{std::nullopt} & \text{if } |N(x)| \neq 1 \end{cases}$$

## Function Signature

```cpp
struct ZPhi {
    int64_t a = 0;
    int64_t b = 0;

    constexpr bool operator==(const ZPhi& o) const = default;
    constexpr ZPhi operator+(const ZPhi& o) const;
    constexpr ZPhi operator-(const ZPhi& o) const;
    constexpr ZPhi operator-() const;
    constexpr ZPhi operator*(const ZPhi& o) const;

    constexpr ZPhi conj() const;
    constexpr int64_t norm() const;
    constexpr std::optional<ZPhi> inv() const;
};
```

## Complexity Target

- **Time:** $O(1)$ integer arithmetic per operation.
- **Space:** $O(1)$.

## Source Reference

- [`golden-integers.md`](file:///home/yassine/code/uor-r4/docs/geometry/golden-integers.md)
