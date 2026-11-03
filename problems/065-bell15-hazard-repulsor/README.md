# 065: Bell-15 Counterfactual Task-Split & $+3\sigma$ Hazard Repulsor

## Problem Statement

When placing a coding or proof task onto the manifold (`place_task`), the planner performs two geometric safety checks without ODE integration overhead:
1. **Bell-15 Counterfactual Task-Split Test:**
   Given the top routed code blocks `block_hits` (already sorted by `similarity` descending):
   - If `block_hits.size() >= 2`, let `#0` be `block_hits[0]` and `#1` be `block_hits[1]`.
   - Compute `sim_gap = std::abs(block_hits[0].similarity - block_hits[1].similarity)`.
   - Two blocks represent a `distinct_concern` if `parent_id` differs OR `primary_arch` differs.
   - Flag `counterfactual_split = true` if and only if:
     $$\text{distinct\_concern} \ \land \ \text{block\_hits}[1].\text{similarity} \ge 0.525 \ \land \ \text{sim\_gap} < \frac{1.0}{15.0}$$
2. **$+3\sigma$ Epistemic Hazard Repulsor:**
   Given a list of `REJECT`-verdict experiment cells and their Hamming distances `hamming_dist` to the task's 8192-bit hypervector:
   - For each rejected cell, compute similarity $\text{sim} = 1.0 - \frac{\text{hamming}}{8192.0}$ and binomial $z$-score:
     $$z = \frac{4096.0 - \text{hamming}}{\sqrt{8192.0} \cdot 0.5} = \frac{4096.0 - \text{hamming}}{45.254833995939045}$$
   - Track the rejected cell with maximum `sim` (`hazard_cell_id`, `hazard_similarity`, `hazard_z_score`).
3. **Placement Decision String:**
   - If `lucidity >= 0.65` and `!block_hits.empty()`: `"INSERT_AT_EXISTING_BLOCK"`
   - Else if `has_best_file`: `"SCAFFOLD_NEW_BLOCK_IN_FILE"`
   - Else: `"SCAFFOLD_NEW_FILE_FOR_STAGE"`

## Mathematical Law / Invariant

- **Bell-15 Margin:** $\theta_{\text{Bell}} = \frac{1}{15} \approx 0.0666667$. Runner-up blocks within the same file AND same archetype never trigger `counterfactual_split`.
- **Standard Normal Scaling:** For an 8192-bit random hypervector ($p = 0.5$), $\mu = 4096$ and $\sigma = \frac{1}{2}\sqrt{8192} = 32\sqrt{2} \approx 45.254834$. A Hamming distance of $3960$ yields $z > +3.0$.

## Function Signature

```cpp
TaskPlacementResult evaluate_bell15_and_hazard(
    double lucidity,
    bool has_best_file,
    const std::vector<RoutedBlockHit>& block_hits,
    const std::vector<RejectedCellCandidate>& reject_cells
);
```

## Complexity Target

- **Time:** $O(N_{\text{reject}})$ single pass.
- **Space:** $O(1)$.

## Source Reference

- [manifold.cpp](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L2323-L2369)
