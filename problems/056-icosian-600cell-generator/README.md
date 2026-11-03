# 056: Binary Icosahedral Group $2I$ (120 Icosians Generator)

## Problem Statement

The **binary icosahedral group** $2I \subset S^3$ consists of 120 unit quaternions (the **icosians**), which form the 120 vertices of the regular 4D **600-cell** and the root system of the non-crystallographic Coxeter group $H_4$. Every coordinate lies in the half-golden-integer ring $\frac{1}{2}\mathbb{Z}[\phi]$ (where $\phi = \frac{1+\sqrt{5}}{2}, \phi^2 = \phi + 1, \phi^{-1} = \phi - 1$), allowing all 120 vertices to be represented with **zero floating-point error** in doubled coordinates $2q \in \mathbb{Z}[\phi]^4$.

## Mathematical Law / Invariant

Represent $x = a + b\phi \in \mathbb{Z}[\phi]$ by `ZPhi{a, b}` with multiplication:
$$(a_1 + b_1\phi)(a_2 + b_2\phi) = (a_1 a_2 + b_1 b_2) + (a_1 b_2 + b_1 a_2 + b_1 b_2)\phi$$
In doubled coordinates $2q \in \mathbb{Z}[\phi]^4$ (where $1 \mapsto (1, 0)$, $\phi \mapsto (0, 1)$, $\phi^{-1} \mapsto (-1, 1)$), the 120 vertices are:
1. **8 Axis Vertices:** permutations of $(\pm 2, 0, 0, 0)$.
2. **16 Hypercube Vertices:** $(\pm 1, \pm 1, \pm 1, \pm 1)$.
3. **96 Golden Vertices:** the 12 **even** permutations of coordinates applied to $(\pm\phi, \pm 1, \pm\phi^{-1}, 0)$ ($12 \times 2^3 = 96$).
4. **Exact Norm & Shell Invariants:**
   - Every doubled icosian $P = 2p$ satisfies $P \cdot P = \sum_{k=0}^3 P_k^2 = 4 + 0\phi$.
   - From any vertex $P \in 2I$, the 120 inner products $P \cdot Q \in \{4, 2\phi, 2, 2\phi^{-1}, 0, -2\phi^{-1}, -2, -2\phi, -4\}$ partition $2I$ into **9 distance shells of sizes $(1, 12, 20, 12, 30, 12, 20, 12, 1)$**.

## Function Signature

```cpp
std::array<Icosian2, 120> generate_icosians_120();
ZPhi icosian_dot4(const Icosian2& p, const Icosian2& q);
std::array<uint32_t, 9> compute_icosian_shell_counts(const std::array<Icosian2, 120>& icosians, size_t ref_idx = 0);
```

## Complexity Target

- **Time:** $O(1)$ exact integer enumeration.
- **Space:** $O(1)$ ($120 \times 32 = 3840$ bytes).

## Source

- [e8-h4-and-the-600-cell.md](file:///home/yassine/code/uor-r4/docs/geometry/e8-h4-and-the-600-cell.md#L11-L20)
- [verify_connections.py](file:///home/yassine/code/uor-r4/docs/figures/geometry/verify_connections.py#L144-L175)
