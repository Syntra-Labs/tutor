# 049: Dyadic Recurrence Gate LUT & Secant Surrogate Credit

## Problem Statement

To eliminate smooth exponential/sigmoid decay gates in recurrent sequence mixers, the **Geometric Recurrence Gate** opens the gate score linearly via $s = \mathrm{clamp}(g + d, 0, 1)$, quantizes $s$ into one of 16 bins $i = \min(\lfloor 16 s \rfloor, 15)$, and reads the dyadic fade rate $\lambda_i = 2^{-(i+1)}$ and unit-circle keep factor $\kappa_i = \sqrt{1 - \lambda_i^2}$ from a 16-entry table. Because hard bin selection has zero derivative almost everywhere, backward training uses the **secant surrogate gradient** across the lawful neighboring bins $i \pm 1$ along the fixed step width $1/16$.

## Mathematical Law / Invariant

For $i \in \{0, \dots, 15\}$:
$$\lambda_i = 2^{-(i+1)}, \qquad \kappa_i = \sqrt{1 - \lambda_i^2}, \qquad \lambda_i^2 + \kappa_i^2 = 1$$

1. **Forward Rate Selection & Recurrence Step:**
   $$s = \mathrm{clamp}(g + d,\; 0.0,\; 1.0), \qquad i = \min(\lfloor 16 s \rfloor,\; 15)$$
   $$h_t = \lambda_i (u_t \otimes h_{t-1}) + \kappa_i b_t$$
2. **Secant Surrogate Credit (`rate_select_secant_backward`):** Given incoming adjoints $(d_\lambda, d_\kappa)$:
   - Initialize $\mathrm{credit} = 0$, $\mathrm{sides} = 0$.
   - If $i > 0$: $\mathrm{credit} \mathrel{-}= d_\lambda (\lambda_{i-1} - \lambda_i) + d_\kappa (\kappa_{i-1} - \kappa_i)$, $\mathrm{sides} \mathrel{+}= 1$.
   - If $i + 1 < 16$: $\mathrm{credit} \mathrel{+}= d_\lambda (\lambda_{i+1} - \lambda_i) + d_\kappa (\kappa_{i+1} - \kappa_i)$, $\mathrm{sides} \mathrel{+}= 1$.
   - Return $\frac{\mathrm{credit}}{\mathrm{sides}} \times 16.0$.

## Function Signature

```cpp
uint32_t rate_index_of(double score);
std::pair<double, double> rate_select_forward(double g, double d);
double rate_select_secant_backward(double g, double d, double d_lambda, double d_kappa);
Quat recurrence_step_forward(const Quat& h_prev, const Quat& u_unit, const Quat& b_t, double g, double d);
```

## Complexity Target

- **Time:** $O(1)$ per step.
- **Space:** $O(1)$ (16-entry static tables).
