# 042: Unit Quaternion Normalization, Conjugate & Inverse on $S^3$

## Problem Statement

A quaternion $q = w + xi + yj + zk \in \mathbb{H} \cong \mathbb{R}^4$ represents a rotor on the 3-sphere $S^3 = \{q \in \mathbb{H} : \|q\|_2 = 1\}$ when normalized. For unit quaternions on $S^3$, the multiplicative inverse $q^{-1}$ coincides with the quaternion conjugate $\bar{q} = w - xi - yj - zk$. Near-zero quaternions ($\|q\|_2 < 10^{-12}$) must safely fall back to the identity rotor $(1, 0, 0, 0)$ without dividing by zero.

## Mathematical Law / Invariant

For $q = (w, x, y, z) \in \mathbb{H}$:
1. **Squared Norm & Norm:** $\|q\|_2^2 = w^2 + x^2 + y^2 + z^2$, $\|q\|_2 = \sqrt{\|q\|_2^2}$.
2. **Projection onto $S^3$:**
   $$\mathrm{normalize}(q) = \begin{cases} (1, 0, 0, 0) & \text{if } \|q\|_2 < 10^{-12}, \\ \frac{1}{\|q\|_2}(w, x, y, z) & \text{otherwise.} \end{cases}$$
3. **Conjugate & Inverse:**
   $$\bar{q} = (w, -x, -y, -z), \qquad q^{-1} = \begin{cases} (1, 0, 0, 0) & \text{if } \|q\|_2^2 < 10^{-24}, \\ \frac{\bar{q}}{\|q\|_2^2} & \text{otherwise.} \end{cases}$$
   On $S^3$ ($\|q\|_2 = 1$), $q^{-1} = \bar{q}$ and $\|\bar{q}\|_2 = 1$.

## Function Signature

```cpp
struct Quat {
    double w{1.0}, x{0.0}, y{0.0}, z{0.0};
    static Quat identity();
    double norm_sq() const;
    double norm() const;
    Quat normalize() const;
    Quat conj() const;
    Quat inverse() const;
};
```

## Complexity Target

- **Time:** $O(1)$ per operation.
- **Space:** $O(1)$ (32 bytes per `Quat`).
