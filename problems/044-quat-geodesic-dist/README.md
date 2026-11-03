# 044: Geodesic Distance on the Double-Cover $S^3$

## Problem Statement

Because $S^3 \cong SU(2)$ is the universal double cover of $SO(3)$, antipodal unit quaternions $q$ and $-q$ represent the exact same 3D rotation. The Riemannian rotation distance on $SO(3) \cong S^3 / \{\pm 1\}$ between two unit quaternions $p, q \in S^3$ is given by twice the acute spherical angle between the lines $\pm p$ and $\pm q$.

## Mathematical Law / Invariant

For unit quaternions $p, q \in S^3$:
$$p \cdot q = p_w q_w + p_x q_x + p_y q_y + p_z q_z$$
$$d_{S^3}(p, q) = 2 \arccos\!\left(\min(1.0,\; |p \cdot q|)\right) \in [0, \pi]$$

Invariants:
1. **Antipodal Identification:** $d_{S^3}(q, -q) = 0$.
2. **Orthogonal Rotors:** If $p \cdot q = 0$ (e.g., $1$ and $i$), then $d_{S^3}(p, q) = \pi$ (a $180^\circ$ physical rotation).
3. **Bi-Invariance on $S^3$:** For any unit rotor $r \in S^3$:
   $$d_{S^3}(r \otimes p,\; r \otimes q) = d_{S^3}(p \otimes r,\; q \otimes r) = d_{S^3}(p, q)$$

## Function Signature

```cpp
double Quat::dot(const Quat& q) const;
double Quat::geodesic_dist(const Quat& q) const;
```

## Complexity Target

- **Time:** $O(1)$ per query.
- **Space:** $O(1)$.

## Source

- [tree_rotor.hpp](file:///home/yassine/code/r4-prover/src/cpp/geometry/tree_rotor.hpp#L61-L72)
