# 039: Great-Circle Haversine Distance on $S^2$

## Problem Statement

On the unit 2-sphere $S^2$, the naive law of cosines $\Delta\sigma = \arccos(\sin\phi_1\sin\phi_2 + \cos\phi_1\cos\phi_2\cos\Delta\lambda)$ suffers catastrophic floating-point cancellation for nearby points ($\Delta\sigma \to 0$). The **Haversine formula** avoids this cancellation by expressing the central angle in terms of $\sin^2(\Delta\phi/2)$ and $\sin^2(\Delta\lambda/2)$, and clamping the intermediate radicand to $[0, 1]$ prevents `NaN` at antipodal points.

## Mathematical Law / Invariant

Given latitude and longitude $(\phi_1, \lambda_1)$ and $(\phi_2, \lambda_2)$ in radians ($\phi \in [-\pi/2, \pi/2]$, $\lambda \in [-\pi, \pi]$):
$$a = \mathrm{clamp}\!\left(\sin^2\!\left(\frac{\phi_2 - \phi_1}{2}\right) + \cos\phi_1 \cos\phi_2 \sin^2\!\left(\frac{\lambda_2 - \lambda_1}{2}\right),\; 0.0,\; 1.0\right)$$
$$\Delta\sigma = 2 \arcsin\!\left(\sqrt{a}\right) \in [0, \pi]$$

Invariants:
1. **Identity & Symmetry:** $\Delta\sigma(p, p) = 0$ and $\Delta\sigma(p, q) = \Delta\sigma(q, p)$.
2. **Antipodal Bound:** $\Delta\sigma((\phi, \lambda), (-\phi, \lambda \pm \pi)) = \pi$.
3. **Triangle Inequality:** $\Delta\sigma(p, r) \le \Delta\sigma(p, q) + \Delta\sigma(q, r)$ for all $p, q, r \in S^2$.

## Function Signature

```cpp
double deg_to_rad(double deg);
double haversine_distance_rad(double lat1_deg, double lon1_deg, double lat2_deg, double lon2_deg);
double haversine_distance_deg(double lat1_deg, double lon1_deg, double lat2_deg, double lon2_deg);
```

## Complexity Target

- **Time:** $O(1)$ per pair (3 trigonometric calls + `sqrt` + `asin`).
- **Space:** $O(1)$.
