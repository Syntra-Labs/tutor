# 047: Hopf Kept-Fibre Angle & 512-Sector Quantizer

## Problem Statement

Because the Hopf projection $h : S^3 \to S^2$ collapses each $S^1$ fibre to a single point on $S^2$, inverting $h(q)$ requires retaining the $S^1$ **kept-fibre phase** $\psi \in [-\pi, \pi]$. In parallel, the 3D base coordinates $(X, Y, Z) \in [-1, 1]^3$ can be quantized into an $8 \times 8 \times 8 = 512$ spatial sector index.

## Mathematical Law / Invariant

Given $q = (w, x, y, z) \in S^3$ and its Hopf projection $(X, Y, Z) = h(q) \in S^2$:
1. **Kept-Fibre Phase Angle:**
   $$\psi(q) = \begin{cases} \mathrm{atan2}(z, w) & \text{if } w^2 + z^2 > 10^{-24}, \\ \mathrm{atan2}(y, x) & \text{otherwise.} \end{cases}$$
2. **Exact Chart Reconstruction ($Z > -1$):** Let $r_1 = \sqrt{\max(0.0, (1 + \mathrm{clamp}(Z, -1, 1)) / 2)}$.
   - If $r_1 > 10^{-12}$:
     $$w = r_1 \cos\psi, \quad z = r_1 \sin\psi, \quad x = \frac{X \sin\psi - Y \cos\psi}{2 r_1}, \quad y = \frac{X \cos\psi + Y \sin\psi}{2 r_1}$$
   - Otherwise ($Z = -1$, south pole): $(w, x, y, z) = (0, \cos\psi, \sin\psi, 0)$.
3. **512-Sector Quantizer:** For each coordinate $c \in \{X, Y, Z\}$, compute bin:
   $$b_c = \min\!\left(\lfloor 8 \cdot \mathrm{clamp}((c + 1) / 2,\; 0.0,\; 1.0) \rfloor,\; 7\right) \in \{0, \dots, 7\}$$
   and pack $\mathrm{sector} = (b_X \ll 6) \mid (b_Y \ll 3) \mid b_Z \in [0, 511]$.

## Function Signature

```cpp
double hopf_fibre_angle(const Quat& q);
Quat reconstruct_from_hopf(const Vec3& s2, double psi);
uint16_t quantize_hopf_sector_512(const Vec3& s2);
```

## Complexity Target

- **Time:** $O(1)$.
- **Space:** $O(1)$.

## Source

- [hopf-observation.md](file:///home/yassine/code/uor-r4/docs/mechanisms/hopf-observation.md)
- [hopf_metric.rs](file:///home/yassine/code/uor-r4/crates/uor-r4-core/src/native_geometric/hopf_metric.rs#L148-L207)
