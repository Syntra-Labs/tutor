# 046: Hopf Fibration Projection ($S^3 \to S^2$) & Fibre Invariance

## Problem Statement

The **Hopf fibration** $S^1 \hookrightarrow S^3 \xrightarrow{h} S^2$ maps a unit quaternion $q = (w, x, y, z) \in S^3$ onto a 3D unit vector on the 2-sphere $S^2$ via $q \mapsto q k \bar{q}$. Because any $U(1)$ phase rotor $e_\psi = (\cos\psi, 0, 0, \sin\psi) = \cos\psi + k\sin\psi$ commutes with $k$, right-multiplying $q$ by $e_\psi$ moves $q$ along a circular $S^1$ fibre in $S^3$ while leaving its shadow $h(q) \in S^2$ strictly invariant.

## Mathematical Law / Invariant

For $q = (w, x, y, z) \in S^3$:
$$h(q) = \begin{pmatrix} 2(xz + wy) \\ 2(yz - wx) \\ w^2 + z^2 - x^2 - y^2 \end{pmatrix} \in S^2$$

Invariants:
1. **Unit-Sphere Landing Identity:**
   $$\|h(q)\|_2^2 = 4(w^2+z^2)(x^2+y^2) + (w^2+z^2 - x^2-y^2)^2 = (w^2+x^2+y^2+z^2)^2 = \|q\|_2^4$$
   Thus $\|q\|_2 = 1 \implies \|h(q)\|_2 = 1$.
2. **Exact $U(1)$ Fibre Gauge Invariance:** For $e_\psi = (\cos\psi, 0, 0, \sin\psi)$ and any $\psi \in \mathbb{R}$:
   $$h(q \otimes e_\psi) = h(q)$$

## Function Signature

```cpp
Vec3 hopf_project(const Quat& q);
Quat hopf_phase_rotor(double psi);
```

## Complexity Target

- **Time:** $O(1)$ (quadratic polynomial in $(w, x, y, z)$, zero trig/sqrt calls for projection).
- **Space:** $O(1)$.
