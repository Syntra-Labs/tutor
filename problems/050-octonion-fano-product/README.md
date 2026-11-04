# 050: Fano Plane Octonion Basis Multiplication

## Problem Statement

The **octonions** $\mathbb{O}$ form the 8-dimensional normed division algebra spanned by $e_0 = 1$ and seven imaginary units $e_1, \dots, e_7$ satisfying $e_i^2 = -1$. Multiplication of distinct imaginary units is governed by the 7 oriented lines of the **Fano plane** $\mathrm{PG}(2, 2)$:
$$\{1,2,4\},\; \{2,3,5\},\; \{3,4,6\},\; \{4,5,7\},\; \{5,6,1\},\; \{6,7,2\},\; \{7,1,3\}$$
Along any cycle $(a, b, c)$, $e_a e_b = +e_c$, $e_b e_c = +e_a$, $e_c e_a = +e_b$, and reversing the order negates the sign.

## Mathematical Law / Invariant

1. **Basis Product (`octonion_basis_product`):** Given `left`, `right` $\in \{0, \dots, 7\}$, returns `{sign, index}` such that $e_{\text{left}} e_{\text{right}} = \text{sign} \cdot e_{\text{index}}$:
   - $e_0 e_k = e_k e_0 = +e_k$
   - $e_k e_k = -e_0$ for $k \in \{1, \dots, 7\}$
   - For $i \neq j \in \{1, \dots, 7\}$, the unique Fano line containing $\{i, j\}$ determines the third index $k$ and orientation sign $\pm 1$.
2. **Non-Associativity Invariant:** Out of all $7 \times 6 \times 5 = 210$ ordered triples $(i, j, k)$ of distinct imaginary units in $\{1, \dots, 7\}$, the $7 \times 6 = 42$ collinear triples lying on a Fano line are associative ($(e_i e_j)e_k = +e_i(e_j e_k)$), while the remaining **$168$ non-collinear triples are strictly anti-associative**:
   $$(e_i e_j)e_k = -e_i(e_j e_k)$$

## Function Signature

```cpp
std::pair<int8_t, uint8_t> octonion_basis_product(uint8_t left, uint8_t right);
uint32_t count_anti_associative_triples();
```

## Complexity Target

- **Time:** $O(1)$ per basis product.
- **Space:** $O(1)$.
