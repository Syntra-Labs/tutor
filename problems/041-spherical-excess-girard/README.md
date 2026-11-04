# 041: Spherical Excess & Girard's Theorem Polygon Area

## Problem Statement

On the unit sphere $S^2$ (Gaussian curvature $K = +1$), the interior angles of a geodesic polygon sum to more than $(n-2)\pi$. By **Girard's Theorem** (the spherical specialization of Gauss-Bonnet), the surface area of a geodesic $n$-gon on $S^2$ equals its **spherical excess** $E$.

## Mathematical Law / Invariant

Given unit vertices $\mathbf{v}_0, \mathbf{v}_1, \dots, \mathbf{v}_{n-1} \in S^2$ of a strictly convex geodesic $n$-gon ($n \ge 3$):
1. At vertex $\mathbf{b}$ with neighbors $\mathbf{a}$ and $\mathbf{c}$, the tangent vectors along the incident great-circle arcs are:
   $$\mathbf{u}_{ba} = \mathbf{a} - (\mathbf{a}\cdot\mathbf{b})\mathbf{b}, \qquad \mathbf{u}_{bc} = \mathbf{c} - (\mathbf{c}\cdot\mathbf{b})\mathbf{b}$$
   and the interior angle is:
   $$\alpha_b = \arccos\!\left(\mathrm{clamp}\!\left(\frac{\mathbf{u}_{ba} \cdot \mathbf{u}_{bc}}{\|\mathbf{u}_{ba}\|_2 \|\mathbf{u}_{bc}\|_2},\; -1.0,\; 1.0\right)\right) \in [0, \pi]$$
2. **Girard's Formula:**
   $$\mathrm{Area}(P) = E = \sum_{i=0}^{n-1} \alpha_i - (n - 2)\pi$$
   For the octant triangle $\{(1,0,0), (0,1,0), (0,0,1)\}$, each angle is $\pi/2$, so $E = \frac{3\pi}{2} - \pi = \frac{\pi}{2} = \frac{1}{8}(4\pi)$.

## Function Signature

```cpp
double spherical_triangle_angle(const Vec3& a, const Vec3& b, const Vec3& c);
double spherical_excess_triangle(const Vec3& a, const Vec3& b, const Vec3& c);
double spherical_excess_polygon(const std::vector<Vec3>& vertices);
```

## Complexity Target

- **Time:** $O(n)$ for an $n$-vertex geodesic polygon.
- **Space:** $O(1)$.
