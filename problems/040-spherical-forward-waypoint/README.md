# 040: Spherical Forward Geodesic Waypoint & Bearing

## Problem Statement

In intrinsic turtle geometry on the unit sphere $S^2$ (as used in `sphere-route`), a state in the unit tangent bundle $UT(S^2)$ consists of a unit position vector $\mathbf{p} \in S^2$ and a unit tangent heading vector $\mathbf{t} \in T_{\mathbf{p}}S^2$ satisfying $\mathbf{p} \cdot \mathbf{t} = 0$. Moving forward advances $(\mathbf{p}, \mathbf{t})$ along the great circle spanned by $\mathbf{p}$ and $\mathbf{t}$, while turning rotates $\mathbf{t}$ in the tangent plane around the surface normal $\mathbf{p}$ via Rodrigues' formula.

## Mathematical Law / Invariant

Starting at the North Pole $\mathbf{p}_0 = (0, 0, 1)$ with heading $\mathbf{t}_0 = (1, 0, 0)$:
1. **`forward(deg)`** ($\theta = \text{deg} \cdot \frac{\pi}{180}$):
   $$\mathbf{p}' = \mathbf{p} \cos\theta + \mathbf{t} \sin\theta, \qquad \mathbf{t}' = -\mathbf{p} \sin\theta + \mathbf{t} \cos\theta$$
   followed by Gram-Schmidt re-orthonormalization ($\hat{\mathbf{p}} = \mathbf{p}'/\|\mathbf{p}'\|$, $\hat{\mathbf{t}} = (\mathbf{t}' - (\mathbf{t}'\cdot\hat{\mathbf{p}})\hat{\mathbf{p}})/\|\dots\|$).
2. **`turn(deg)`** ($\alpha = \text{deg} \cdot \frac{\pi}{180}$, positive = left turn around outward normal $\mathbf{p}$):
   $$\mathbf{t}' = \mathbf{t} \cos\alpha + (\mathbf{p} \times \mathbf{t}) \sin\alpha$$
3. ** tangent-bundle invariant:** $\|\mathbf{p}\|_2 = 1$, $\|\mathbf{t}\|_2 = 1$, and $\mathbf{p} \cdot \mathbf{t} = 0$ at every step. Executing `[Forward(90), Turn(90)]` three times traces the octant triangle and returns to $\mathbf{p}_0 = (0,0,1), \mathbf{t}_0 = (1,0,0)$.

## Function Signature

```cpp
void step_forward(SphericalTurtle& state, double deg);
void step_turn(SphericalTurtle& state, double deg);
SphericalTurtle run_spherical_path(const std::vector<Command>& commands);
```

## Complexity Target

- **Time:** $O(N)$ for $N$ commands.
- **Space:** $O(1)$ auxiliary state.
