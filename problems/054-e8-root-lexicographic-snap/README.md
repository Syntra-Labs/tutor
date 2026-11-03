# 054: 16-Root $E_8$ Lexicographic Snapper ($L_1 + \text{Hamming}$)

## Problem Statement

In `SpiralCoreGeometry`, an arbitrary 8D integer vector $x \in \mathbb{Z}^8$ is quantized ("snapped") onto one of the 16 roots $r_0, \dots, r_{15} \in \mathcal{O}_{16}$ (optionally scaled by a multiplicity factor $n \ge 1$) using a purely multiplier-free integer cost function: primary **$L_1$ Manhattan distance**, secondary **coordinate Hamming distance**, and tertiary **smallest root index $r \in [0, 15]$**.

## Mathematical Law / Invariant

Given $x \in \mathbb{Z}^8$, root $r \in [0, 15]$, and scale $n \ge 1$:
1. **Scaled Cost Metrics:** Let $y_d = n \cdot \mathrm{roots}[r][d]$ for $d \in \{0, \dots, 7\}$:
   $$d_{L_1}(x, y) = \sum_{d=0}^7 |x_d - y_d|, \qquad d_H(x, y) = \sum_{d=0}^7 [x_d \neq y_d]$$
2. **Lexicographic Minimization (`snap_e8_root`):**
   $$\mathrm{snap}(x, n) = \arg\min_{r \in \{0,\dots,15\}} \bigl(d_{L_1}(x, n\cdot\mathrm{roots}[r]),\; d_H(x, n\cdot\mathrm{roots}[r]),\; r\bigr)$$
3. **Exact Idempotence Invariant:** For any $r \in [0, 15]$ and $n \ge 1$, $\mathrm{snap}(n \cdot \mathrm{roots}[r], n) = r$ with $(d_{L_1}, d_H) = (0, 0)$.

## Function Signature

```cpp
void root_cost(
    const std::array<std::array<int8_t, 8>, 16>& roots,
    const int32_t x[8],
    uint8_t root_idx,
    uint32_t& l1_dist,
    uint32_t& hamming
);
uint8_t snap_e8_root(
    const std::array<std::array<int8_t, 8>, 16>& roots,
    const int32_t x[8],
    uint32_t scale = 1
);
```

## Complexity Target

- **Time:** $O(1)$ ($16 \times 8 = 128$ integer comparisons/subtractions).
- **Space:** $O(1)$.

## Source

- [manifold.cpp](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L350-L382)
