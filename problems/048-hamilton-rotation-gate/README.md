# 048: Hamilton Rotation Gate Forward & Exact Adjoint Backward

## Problem Statement

The **Hamilton Rotation Gate** replaces squashing feed-forward gates (such as sigmoid/SwiGLU) with a norm-preserving 4D quaternion rotation. Given a 4D gate block $g$ and a 4D value ("up") block $u$, it normalizes $g$ with a $10^{-12}$ guard core and rotates $u$ via the left Hamilton product $q \otimes u$. Both forward and backward passes are algebraic and free of exponentials or transcendental functions.

## Mathematical Law / Invariant

Given gate $g = (g_0, g_1, g_2, g_3)$ and value $u = (u_0, u_1, u_2, u_3)$:
1. **Forward Pass:**
   $$r = \sqrt{\|g\|_2^2 + 10^{-12}}, \qquad q = \frac{g}{r}, \qquad \mathrm{out} = q \otimes u$$
2. **Exact Backward Adjoint:** Given upstream gradient $d = \partial\mathcal{L}/\partial\mathrm{out}$:
   $$\partial u = \bar{q} \otimes d, \qquad \partial q = d \otimes \bar{u}, \qquad \partial g = \frac{\partial q - q(q \cdot \partial q)}{r}$$
3. **Invariants:**
   - **Norm Preservation:** $\|\mathrm{out}\|_2 = \|q\|_2 \|u\|_2 = \frac{\|g\|_2}{\sqrt{\|g\|_2^2 + 10^{-12}}} \|u\|_2$.
   - **Tangent Projection:** For any $\epsilon \to 0$, $\partial g$ matches central finite differences to $O(\epsilon^2)$, and in the limit $\|q\|_2 \to 1$, $q \cdot \partial g \to 0$.

## Function Signature

```cpp
Quat hamilton_gate_forward(const Quat& g, const Quat& u);
HamiltonGateGrads hamilton_gate_backward(const Quat& g, const Quat& u, const Quat& d_out);
```

## Complexity Target

- **Time:** $O(1)$ per 4-block.
- **Space:** $O(1)$.

## Source

- [hamilton-gate.md](file:///home/yassine/code/uor-r4/docs/mechanisms/hamilton-gate.md#L19-L30)
- [cuda_stack.cu](file:///home/yassine/code/uor-r4/crates/uor-r4-training/src/cuda_stack.cu#L172-L220)
