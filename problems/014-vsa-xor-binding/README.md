# 014: VSA XOR Binding & Self-Inverse Unbinding

## Problem Statement

In binary Vector Symbolic Architectures (VSA / BSC), the **binding** operator $\otimes$ associates a *role* (key) hypervector $R$ with a *filler* (value) hypervector $F$ via bitwise XOR across all 8,192 bits.

Implement:
1. `Hypervec8192 Hypervec8192::bind(const Hypervec8192& other) const`:
   - Compute `out.words[i] = words[i] ^ other.words[i]` for $i \in [0, 127]$.
2. `size_t unbind_and_match(const Hypervec8192& trace, const Hypervec8192& role, std::span<const Hypervec8192> candidates)`:
   - Unbind `role` from `trace` via `Hypervec8192 query = trace.bind(role)` (since XOR is its own inverse), and return the index $k$ in `candidates` that minimizes `query.hamming(candidates[k])`.

## Mathematical Law / Invariant

- **Abelian Group & Self-Inverse ($x \oplus x = 0$):** $(\{0,1\}^{8192}, \oplus)$ is a Boolean group of exponent 2:
  $$(R \oplus F) \oplus R = F$$
- **Hamming Distance Isometry:** Binding by any context vector $C$ preserves exact pairwise Hamming distances:
  $$d_H(A \oplus C, B \oplus C) = d_H(A, B)$$
- **Dissimilarity to Operands:** For random $A, B \in \{0,1\}^{8192}$, the bound vector $A \oplus B$ is quasi-orthogonal to both $A$ and $B$ ($|z(A \oplus B, A)| < 3.5$).

## Function Signature

```cpp
struct Hypervec8192 {
    static constexpr size_t HV_BITS = 8192;
    static constexpr size_t HV_WORDS = 128;
    std::array<uint64_t, HV_WORDS> words{};

    static Hypervec8192 from_seed(uint64_t seed);
    Hypervec8192 bind(const Hypervec8192& other) const;
    uint32_t hamming(const Hypervec8192& other) const;
    double z_score(const Hypervec8192& other) const;
};

size_t unbind_and_match(
    const Hypervec8192& trace,
    const Hypervec8192& role,
    std::span<const Hypervec8192> candidates
);
```

## Complexity Target

- **Time:** $O(128)$ words for `bind` (16 AVX-512 `vpxorq` or 32 AVX2 `vpxor` instructions); $O(K \times 128)$ for candidate lookup.
- **Space:** $O(1)$ auxiliary space.
