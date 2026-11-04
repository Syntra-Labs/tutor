# 020: Inline 3-Block Triad Majority Consensus Signature

## Problem Statement

In base-3 structural indexing (`SpiralCore` triad folding), every consecutive group of 3 blocks $(c_0, c_1, c_2)$ is folded inline into a single 256-bit **Triad Consensus Signature** without allocating a heap tree:
$$\text{maj}(a, b, c) = (a \land b) \lor (b \land c) \lor (a \land c)$$

Implement:
1. `HelixSignature256 triad_majority_signature(const HelixSignature256& s0, const HelixSignature256& s1, const HelixSignature256& s2)`:
   - Apply the 3-way bitwise majority operator across all four 64-bit quadrants (`s0, s1, s2, s3`).
2. `uint32_tHelixSignature256::hamming(const HelixSignature256& other) const`:
   - Return the 256-bit Hamming distance across the 4 words.

## Mathematical Law / Invariant

- **Hypercube Steiner Median Identity:** On the 256-dimensional binary hypercube $\{0, 1\}^{256}$, $M = \text{maj}(A, B, C)$ is the unique point minimizing total Hamming distance to $(A, B, C)$, and satisfies the exact identity:
  $$d_H(M, A) + d_H(M, B) + d_H(M, C) = \frac{1}{2}\Big(d_H(A, B) + d_H(B, C) + d_H(A, C)\Big) \le 256$$
- **Two-Child Proximity Bound:** Because $d_H(M, A) + d_H(M, B) + d_H(M, C) \le 256$, at least two of the three children are guaranteed to be within Hamming distance $\le 128$ of $M$ (and for random independent signatures, expected distance is $256 \times \frac{1}{4} = 64$ bits to every child!).

## Function Signature

```cpp
struct HelixSignature256 {
    uint64_t s0 = 0;
    uint64_t s1 = 0;
    uint64_t s2 = 0;
    uint64_t s3 = 0;

    uint32_t hamming(const HelixSignature256& other) const;
};

HelixSignature256 triad_majority_signature(
    const HelixSignature256& s0,
    const HelixSignature256& s1,
    const HelixSignature256& s2
);
```

## Complexity Target

- **Time:** $O(1)$ — 20 bitwise instructions (5 per 64-bit word, or 3 with ternary logic `vpternlogq`).
- **Space:** 32 bytes.
