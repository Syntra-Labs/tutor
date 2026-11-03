# 015: 8192-Bit Cross-Word Cyclic Permutation ($\rho^s$)

## Problem Statement

In Vector Symbolic Architectures, non-commutative sequential order (e.g., $n$-grams, AST depth, causal time steps) is encoded using a cyclic bit-permutation operator $\rho^s$ across all 8,192 bits ($128 \times 64$-bit words).

Implement `Hypervec8192 Hypervec8192::rotl(uint32_t shift) const`:
- Reduce `shift %= 8192`. If `shift == 0`, return `*this`.
- Decompose `word_shift = shift / 64` and `bit_shift = shift % 64`.
- For each word $i \in [0, 127]$:
  - `src_hi = (i + 128 - word_shift) % 128`
  - If `bit_shift == 0`: `out.words[i] = words[src_hi]`
  - Else: `src_lo = (src_hi + 127) % 128`, and `out.words[i] = (words[src_hi] << bit_shift) | (words[src_lo] >> (64 - bit_shift))`

## Mathematical Law / Invariant

- **Cyclic Group Homomorphism ($\mathbb{Z}_{8192}$):**
  $$\rho^a(\rho^b(x)) = \rho^{(a + b) \bmod 8192}(x), \quad \rho^{8192}(x) = x$$
- **Distributivity over Binding ($\oplus$) & Isometry:**
  $$\rho^s(A \oplus B) = \rho^s(A) \oplus \rho^s(B), \quad d_H(\rho^s(A), \rho^s(B)) = d_H(A, B)$$
- **Quasi-Orthogonality of Non-Zero Shifts:** For a random hypervector $A$ and any $s \not\equiv 0 \pmod{8192}$, $\rho^s(A)$ is quasi-orthogonal to $A$ ($|z(\rho^s(A), A)| < 3.5$).

## Function Signature

```cpp
struct Hypervec8192 {
    static constexpr size_t HV_BITS = 8192;
    static constexpr size_t HV_WORDS = 128;
    std::array<uint64_t, HV_WORDS> words{};

    static Hypervec8192 from_seed(uint64_t seed);
    Hypervec8192 rotl(uint32_t shift) const;
    uint32_t hamming(const Hypervec8192& other) const;
};
```

## Complexity Target

- **Time:** $O(128)$ words (~60–90 CPU cycles).
- **Space:** $O(1)$ auxiliary registers.

## Source

- [`manifold.cpp` (lines 450–466)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L450-L466)
