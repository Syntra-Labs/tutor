# 036: Prime Token Factor Set & Exact GCD Set Intersection

## Problem Statement

By the Fundamental Theorem of Arithmetic (unique prime factorization), assigning each token ID $k \in \{0, 1, \dots, 14\}$ its own prime $p_k$ ($p_0 = 2, p_1 = 3, p_2 = 5, \dots, p_{14} = 47$) turns finite token sets into square-free integer products:

$$P(A) = \prod_{k \in A} p_k$$

Note that inside 64-bit unsigned integers, the product of the first 15 primes $\prod_{k=0}^{14} p_k = 614,\!889,\!782,\!588,\!491,\!410 < 2^{64}$ never overflows!

Implement:
1. `nth_prime(size_t k)`: returns the $k$-th prime ($0$-indexed: $p_0 = 2, p_1 = 3, \dots$).
2. `encode_prime_set(const std::vector<size_t>& token_ids)`: computes the square-free product $P(A) = \prod_{k \in \text{unique}(A)} p_k$ (ignoring duplicate occurrences of the same token ID so $P(A)$ is square-free).
3. `intersect_prime_sets(uint64_t prod_a, uint64_t prod_b)`: computes $g = \gcd(P_A, P_B)$ and recovers the sorted vector of shared token IDs $\{k : p_k \mid g\}$.

## Mathematical Law / Invariant

For any two sets of token IDs $A, B \subseteq \{0, \dots, 14\}$:

- **GCD Intersection Isomorphism:**
  $$\gcd(P(A), P(B)) = P(A \cap B) = \prod_{k \in A \cap B} p_k$$
- **LCM Union Isomorphism:**
  $$\operatorname{lcm}(P(A), P(B)) = \frac{P(A) \cdot P(B)}{\gcd(P(A), P(B))} = P(A \cup B)$$
- **Worked Example (`primes-and-zeta.md`):**
  With tokens $\{0, 1, 3\} \mapsto 2 \cdot 3 \cdot 7 = 42$ and $\{1, 2, 3\} \mapsto 3 \cdot 5 \cdot 7 = 105$, $\gcd(42, 105) = 21 = 3 \cdot 7$, which factors into token IDs $\{1, 3\}$.

## Function Signature

```cpp
constexpr uint64_t nth_prime(size_t k);
uint64_t encode_prime_set(const std::vector<size_t>& token_ids);
std::vector<size_t> intersect_prime_sets(uint64_t prod_a, uint64_t prod_b);
```

## Complexity Target

- **Time:** $O(\log(\min(P_A, P_B)))$ for Euclidean GCD plus $O(|V|)$ trial division over the prime table.
- **Space:** $O(1)$ auxiliary space.

## Source Reference

- [`primes-and-zeta.md` (lines 30–37)](file:///home/yassine/code/uor-r4/docs/geometry/primes-and-zeta.md#L30-L37)
