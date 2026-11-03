# 058: $E_8 = H_4 \oplus \phi H_4$ Reduced-Metric Bilinear Form

## Problem Statement

Pairing the 120 unit icosians $2I$ (the $H_4$ roots) with a golden-ratio-scaled copy $\phi \cdot 2I$ yields $120 + 120 = 240$ elements in $\frac{1}{2}\mathbb{Z}[\phi]^4$. Endowing this 8-dimensional $\mathbb{Z}$-module with the **reduced-metric bilinear form** $\langle x, y \rangle = 2a$ (where the quaternion dot product is $x \cdot y = a + b\phi$) turns $2I \cup \phi \cdot 2I$ into the **240 roots of the $E_8$ lattice** with norm $2$, exact $E_8$ inner-product shell distribution $(1, 56, 126, 56, 1)$, and closure under Weyl reflections!

## Mathematical Law / Invariant

In doubled coordinates $A = 2x, B = 2y \in \mathbb{Z}[\phi]^4$:
1. **240 Roots ($2I \cup \phi \cdot 2I$):**
   The first 120 elements are $A \in 2I$, and the next 120 elements are $\phi A = (\phi A_0, \phi A_1, \phi A_2, \phi A_3)$ where $\phi = (0, 1) \in \mathbb{Z}[\phi]$.
2. **Reduced-Metric Inner Product (`e8_reduced_inner_product`):**
   Let $\sum_{k=0}^3 A_k B_k = 4(x \cdot y) = m + n\phi \in \mathbb{Z}[\phi]$. Because $x \cdot y = \frac{m}{4} + \frac{n}{4}\phi$, the reduced bilinear form $\langle x, y \rangle = 2 \cdot \mathrm{rational\_part}(x \cdot y)$ is:
   $$\langle x, y \rangle = \frac{m}{2} \in \{-2, -1, 0, +1, +2\}$$
3. **$E_8$ Inner-Product Shell Counts:**
   From any of the 240 roots $r \in 2I \cup \phi \cdot 2I$, the values of $\langle r, s \rangle$ across all $s \in 2I \cup \phi \cdot 2I$ have counts:
   $$(+2 : 1,\quad +1 : 56,\quad 0 : 126,\quad -1 : 56,\quad -2 : 1)$$
4. **Weyl Reflection Closure (`e8_weyl_reflect`):**
   For any roots $x, r \in 2I \cup \phi \cdot 2I$, the reflection $w_r(x) = x - \langle x, r \rangle r$ also belongs to $2I \cup \phi \cdot 2I$.

## Function Signature

```cpp
std::array<Icosian2, 240> build_e8_golden_roots_240(const std::array<Icosian2, 120>& icosians);
int32_t e8_reduced_inner_product(const Icosian2& A, const Icosian2& B);
std::array<uint32_t, 5> e8_inner_product_distribution(const std::array<Icosian2, 240>& roots, size_t ref_idx);
Icosian2 e8_weyl_reflect(const Icosian2& x, const Icosian2& r);
```

## Complexity Target

- **Time:** $O(1)$ per inner product or reflection.
- **Space:** $O(1)$ ($240 \times 32 = 7680$ bytes).

## Source

- [e8-h4-and-the-600-cell.md](file:///home/yassine/code/uor-r4/docs/geometry/e8-h4-and-the-600-cell.md#L25-L38)
- [verify_connections.py](file:///home/yassine/code/uor-r4/docs/figures/geometry/verify_connections.py#L335-L374)
