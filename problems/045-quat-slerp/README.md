# 045: Spherical Linear Interpolation (SLERP) with Small-Angle Fallback

## Problem Statement

Given two unit quaternions $q_1, q_2 \in S^3$ and an interpolation parameter $t \in [0, 1]$, **Spherical Linear Interpolation (SLERP)** traces the constant-angular-velocity shortest geodesic arc on $S^3 / \{\pm 1\}$. When $q_1 \cdot q_2 < 0$, $q_2$ must be negated to ensure the shortest path on $SO(3)$; when $|q_1 \cdot q_2| > 0.9995$, $\sin\theta_0 \to 0$ and the formula must fall back to normalized linear interpolation (NLERP) for numerical stability.

## Mathematical Law / Invariant

Given unit quaternions $q_1, q_2 \in S^3$ and $t \in [0, 1]$:
1. Compute $d = q_1 \cdot q_2$. If $d < 0$, set $q_2 \leftarrow -q_2$ and $d \leftarrow -d$.
2. If $d > 0.9995$, return $\mathrm{normalize}((1 - t) q_1 + t q_2)$.
3. Otherwise, let $\theta_0 = \arccos(\min(1.0, d))$ and $\theta = t \theta_0$:
   $$\mathrm{slerp}(q_1, q_2, t) = \frac{\sin(\theta_0 - \theta)}{\sin\theta_0} q_1 + \frac{\sin\theta}{\sin\theta_0} q_2$$
4. **Constant Angular Speed Invariant:**
   $$d_{S^3}(\mathrm{slerp}(q_1, q_2, t), q_1) = t \cdot d_{S^3}(q_1, q_2), \qquad \|\mathrm{slerp}(q_1, q_2, t)\|_2 = 1$$

## Function Signature

```cpp
Quat slerp(Quat q1, Quat q2, double t);
```

## Complexity Target

- **Time:** $O(1)$ per interpolation.
- **Space:** $O(1)$.
