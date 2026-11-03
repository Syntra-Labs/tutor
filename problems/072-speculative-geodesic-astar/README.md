# 072: Speculative Geodesic A* Search over Proof States

## Problem Statement

`SpeculativeGeodesicRouter` and `SpeculativeGeodesicProver` (`src/cpp/search/geodesic_search.cpp`) guide proof search along the direction of steepest descent in proof space while minimizing expensive Lean REPL kernel calls via margin-gated speculation.

Implement:
1. **`SpeculativeGeodesicRouter::rank_candidates(tangent_vector, action_dict)`:**
   - If $\|\text{tangent\_vector}\|_2 < 10^{-9}$, throw `std::invalid_argument`.
   - For each `(name, vec)` in `action_dict`, compute cosine similarity:
     $$\text{score} = \begin{cases} \frac{\langle \text{tangent\_vector}, \text{vec} \rangle}{\|\text{tangent\_vector}\|_2 \|\text{vec}\|_2} & \text{if } \|\text{vec}\|_2 \ge 10^{-9} \\ 0.0 & \text{otherwise} \end{cases}$$
   - Sort candidates by `(score descending, name ascending)` for deterministic tie-breaking.
2. **`SpeculativeGeodesicRouter::select_speculative_queue(ranked)`:**
   - If `ranked.empty()`, return `({}, 0.0, false)`.
   - If `ranked.size() == 1`, return `({ranked[0]}, 1.0, false)`.
   - Let $\text{margin} = \text{ranked}[0].\text{score} - \text{ranked}[1].\text{score}$.
   - If $\text{margin} \ge \text{margin\_threshold\_}$ or $\text{max\_beam\_width\_} < 2$, commit solely to `{ranked[0]}` (`is_speculative = false`); otherwise return `{ranked[0], ranked[1]}` (`is_speculative = true`).
3. **`SpeculativeGeodesicProver::search(initial_goal, policy_fn, action_dict, kernel_eval_fn)`:**
   - Maintain a best-first open queue ordered by $f(s) = \text{depth} + h(s)$ (with FIFO tie-breaking) and a `visited` set of expanded `goal_str` states to avoid cycles.
   - At each step (up to `max_steps_`), pop the best open node `curr`. If `curr->depth >= max_depth_`, skip it.
   - Compute `tangent_v = policy_fn(curr->goal_str)`, rank candidates, and select the speculative queue `queue`.
   - Iterate `idx = 0..queue.size()-1`:
     - Increment `kernel_calls` and invoke `kernel_eval_fn(curr->goal_str, queue[idx].name)`.
     - If `GoalsClosed`: if `idx == 0 && queue.size() > 1`, increment `repl_calls_saved`. Reconstruct the winning root-to-leaf tactic script by walking parent pointers and return `SuccessProven`.
     - If `SubgoalsOpen`: if `idx == 0 && queue.size() > 1`, increment `repl_calls_saved`. Push unvisited child goals at `depth = curr->depth + 1` with `parent = curr` and `applied_tactic = queue[idx].name`, then `break` out of the speculative candidate loop.
     - If `KernelError`: continue to the next fallback candidate in `queue`.

## Mathematical Law / Invariant

- **Margin-Gated Speculation:** When the top tactic leads the runner-up by $\ge \theta_{\text{margin}}$, only 1 candidate is queued; when within $\theta_{\text{margin}}$ and candidate #0 succeeds on the first try, `repl_calls_saved` increments by 1.
- **Exact Path Reconstruction:** Upon `GoalsClosed`, `proof_steps` contains the exact sequence of tactics along the winning root-to-goal branch (excluding dead-end branches).

## Function Signature

```cpp
class SpeculativeGeodesicRouter {
public:
    std::vector<TacticCandidate> rank_candidates(
        const std::vector<double>& tangent_vector,
        const std::unordered_map<std::string, std::vector<double>>& action_dict
    ) const;

    std::tuple<std::vector<TacticCandidate>, double, bool> select_speculative_queue(
        const std::vector<TacticCandidate>& ranked
    ) const;
};
```

## Complexity Target

- **Time:** $O(B \cdot |A| \cdot D)$ per search step where $|A|$ is action dictionary size and $D$ is tangent dimension.
- **Space:** $O(\text{max\_steps})$ search tree nodes.

## Source Reference

- [geodesic_search.cpp](file:///home/yassine/code/r4-prover/src/cpp/search/geodesic_search.cpp)
- [geodesic_search.hpp](file:///home/yassine/code/r4-prover/src/cpp/search/geodesic_search.hpp)
