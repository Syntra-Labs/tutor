# 064: Manifold Lucidity Score & Surprise-Gated Delta Learning

## Problem Statement

When routing a task or query against the manifold, the system computes a **Lucidity Score** measuring how well the Best Matching Unit (BMU) and lifecycle archetype explain the query, and applies a **Surprise-Gated Local Delta Rule** to update archetype accumulators only when prediction surprise exceeds a margin.

Implement two functions:
1. `LucidityEval compute_lucidity_eval(double bmu_sim, double archetype_sim)`:
   - Normalized excess BMU resonance:
     $$\text{excess\_bmu} = \text{clamp}\!\left(\frac{\text{bmu\_sim} - 0.52}{0.10},\ 0.0,\ 1.0\right)$$
   - Quantization error:
     $$q_{\text{err}} = (1.0 - \text{excess\_bmu}) \cdot 1.5, \qquad q_{\text{norm}} = \tanh(q_{\text{err}})$$
   - Archetype resonance position:
     $$\text{cos\_pos} = \text{clamp}\!\left(\frac{\text{archetype\_sim} - 0.50}{0.08},\ 0.0,\ 1.0\right)$$
   - Combined lucidity score:
     $$\text{lucidity} = \text{clamp}\!\left(1.0 - 0.5 \cdot q_{\text{norm}} + 0.5 \cdot \text{cos\_pos} - 0.25,\ 0.0,\ 1.0\right)$$

2. `DeltaFeedback compute_surprise_delta(double query_target_sim, bool positive_reward)`:
   - Compute prediction surprise:
     $$\text{surprise} = \text{clamp}(1.0 - \text{query\_target\_sim},\ 0.0,\ 1.0)$$
   - If $\text{surprise} < 0.35$, no weight update occurs (`updated = false`, `delta_w = 0`).
   - If $\text{surprise} \ge 0.35$, set `updated = true` and compute integer weight magnitude:
     $$w = \text{clamp}(\text{llround}(4.0 \cdot \text{surprise}),\ 1,\ 4)$$
     returning `delta_w = positive_reward ? +w : -w`.

## Mathematical Law / Invariant

- **Boundedness:** `q_error` $\in [0.0, 1.5]$ and `lucidity` $\in [0.0, 1.0]$ for all inputs.
- **High-Resonance Fixed Point:** When $\text{bmu\_sim} \ge 0.62$ and $\text{archetype\_sim} \ge 0.58$, $q_{\text{err}} = 0.0$ and $\text{lucidity} = 1.0$.
- **Surprise Gate Deadzone:** Queries with $\text{sim} > 0.65$ ($\text{surprise} < 0.35$) never trigger accumulator writes (`delta_w == 0`).

## Function Signature

```cpp
LucidityEval compute_lucidity_eval(double bmu_sim, double archetype_sim);
DeltaFeedback compute_surprise_delta(double query_target_sim, bool positive_reward = true);
```

## Complexity Target

- **Time:** $O(1)$ (< 10 ns/op).
- **Space:** $O(1)$.
