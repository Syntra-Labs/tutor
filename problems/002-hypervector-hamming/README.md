# 002: 8192-bit Hypervector Hamming Distance

## Specification & Contract

In dense binary hyperdimensional computing (HDC) and geometric substrates, representations are vectors of 8192 bits packed into $128 \times \text{uint64\_t}$ words.

Implement:
1. `size_t hamming_distance(const Hypervec8192& a, const Hypervec8192& b)`:
   - Count the number of differing bits across the two 8192-bit vectors (XOR + popcount).
2. `double cosine_similarity(const Hypervec8192& a, const Hypervec8192& b)`:
   - Normalized geometric similarity: $1.0 - \frac{2 \cdot \text{hamming\_distance}}{8192}$.
   - Identical vectors return $1.0$. Orthogonal vectors return $0.0$. Inverse vectors return $-1.0$.
