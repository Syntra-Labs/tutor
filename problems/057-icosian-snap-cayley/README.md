# 057: 600-Cell Voronoi Snapper & $120 \times 120$ Cayley Table

## Problem Statement

The 120 unit icosians form a closed non-abelian subgroup $2I \subset S^3$ under quaternion multiplication. Precomputing the $120 \times 120$ Cayley multiplication table turns 4D quaternion composition into a single 1-byte table lookup, while **Voronoi snapping** (`snap_icosian`) projects any continuous quaternion $q \in \mathbb{R}^4$ onto its nearest 600-cell vertex by maximizing the 4D Euclidean inner product.

## Mathematical Law / Invariant

1. **Exact Doubled Icosian Product (`icosian_mul`):**
   Given doubled icosians $A = 2p, B = 2q \in \mathbb{Z}[\phi]^4$, their raw Hamilton product $A \otimes B = 4(p \otimes q)$ has strictly even integer coefficients in $\mathbb{Z}[\phi]^4$. Dividing each component by $2$ yields $2(p \otimes q) \in 2I$.
2. **$120 \times 120$ Cayley Table (`build_icosian_cayley_table`):**
   For all $i, j \in [0, 119]$, $\mathrm{cayley}[i][j] \in [0, 119]$ is the unique index $k$ such that $\mathrm{icosians}[k] = \mathrm{icosian\_mul}(\mathrm{icosians}[i], \mathrm{icosians}[j])$. Every row and column of `cayley` is a permutation of $\{0, \dots, 119\}$ (the Latin square property of finite groups).
3. **600-Cell Voronoi Snapper (`snap_icosian`):**
   $$\mathrm{snap\_icosian}(q) = \arg\max_{i \in \{0,\dots,119\}} \sum_{c=0}^3 q_c \cdot \mathrm{to\_double}(\mathrm{icosians}[i][c])$$
   (tie-breaking to the smallest index $i$).

## Function Signature

```cpp
Icosian2 icosian_mul(const Icosian2& A, const Icosian2& B);
std::array<std::array<uint8_t, 120>, 120> build_icosian_cayley_table(const std::array<Icosian2, 120>& icosians);
uint8_t snap_icosian(const std::array<Icosian2, 120>& icosians, double w, double x, double y, double z);
```

## Complexity Target

- **Time:** $O(1)$ per Cayley lookup; $120$ dot products per `snap_icosian`.
- **Space:** $O(1)$ ($14,400$ bytes for the $120 \times 120$ table).

## Source

- [spin-weights.md](file:///home/yassine/code/uor-r4/docs/mechanisms/spin-weights.md)
- [verify_connections.py](file:///home/yassine/code/uor-r4/docs/figures/geometry/verify_connections.py#L129-L161)
