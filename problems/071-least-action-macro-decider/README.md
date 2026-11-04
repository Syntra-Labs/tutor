# 071: Least-Action Macro-Action Decider

## Problem Statement

During neurosymbolic theorem proving, `MacroActionDecider` (`src/cpp/search/macro_decider.cpp`) routes a proof state `GoalState` between two macro-action tiers:
- **Tier 2A (`Tier2A_DirectTerminating`):** Direct closing tactics `{"exact", "rfl", "simp", "omega", "ring", "linarith", "decide"}`.
- **Tier 2B (`Tier2B_ForwardInvention`):** Exploratory multi-step tactics `{"apply", "rw", "intro", "cases", "induction"}`.

Implement:
1. **`bool MacroActionDecider::is_reflexive_or_terminal_head(const std::string& conclusion)`:**
   - Trim whitespace and strip any prefix up to and including `"⊢"` (3 UTF-8 bytes `"\xe2\x8a\xa2"`).
   - Return `true` if the remaining conclusion is `"True"`, `"true"`, `"⊤"`, `"rfl"`, `"Eq.refl"`, or `"refl"`.
   - Otherwise check for equality (`" = "`, `"=="`, or single `'='` not preceded by `'!'`, `'<'`, `'>'`): return `true` if the trimmed LHS is non-empty and equals the trimmed RHS (`lhs == rhs`).
2. **`MacroRouteDecision MacroActionDecider::route(const GoalState& state) const`:**
   - Compute $d_{S^3}(z_s, z_{\text{qed}}) = 2 \arccos(|\langle \hat{z}_s, \hat{z}_{\text{qed}} \rangle|)$.
   - If `is_reflexive_or_terminal_head(state.conclusion)` is `true` OR $d_{S^3}(z_s, z_{\text{qed}}) \le \theta_{\text{conv}}$, route to `Tier2A_DirectTerminating` with Tier 2A candidate tactics; otherwise route to `Tier2B_ForwardInvention` with Tier 2B candidate tactics.
3. **`MacroRouteDecision MacroActionDecider::route_with_feedback(const GoalState& state, const std::function<bool(const std::string&)>& kernel_eval) const`:**
   - Call `route(state)`. If routed to Tier 2A, test each Tier 2A tactic with `kernel_eval(tac)` in order; if none succeeds, fall back to `Tier2B_ForwardInvention` with `fallback_triggered = true`.

## Mathematical Law / Invariant

- **Syntactic Reflexivity Dominance:** Any goal of the form `⊢ E = E` or `⊢ True` routes to Tier 2A regardless of its quaternion embedding $z_s$.
- **Geodesic Convergence Ball:** Any state within $d_{S^3}(z_s, z_{\text{qed}}) \le \theta_{\text{conv}}$ routes to Tier 2A initially, and automatically falls back to Tier 2B if all Tier 2A closing tactics are rejected by the kernel.

## Function Signature

```cpp
class MacroActionDecider {
public:
    static bool is_reflexive_or_terminal_head(const std::string& conclusion);
    MacroRouteDecision route(const GoalState& state) const;
    MacroRouteDecision route_with_feedback(
        const GoalState& state,
        const std::function<bool(const std::string&)>& kernel_eval
    ) const;
};
```

## Complexity Target

- **Time:** $O(|C|)$ string scan + $O(1)$ quaternion geodesic distance.
- **Space:** $O(1)$.
