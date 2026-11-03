# 070: Non-Commutative Tree Rotor DAG Folder on $S^3$

## Problem Statement

Flat token sequence encoders truncate deep mathematical syntax trees and fail to distinguish non-commutative argument order cleanly. The `TreeRotorOperator` (`src/cpp/geometry/tree_rotor.cpp`) folds an arbitrary `Lean.Expr` DAG bottom-up into the unit 3-sphere $S^3 \subset \mathbb{H}$ with **zero window truncation** and **memoized $O(|V|)$ DAG traversal**.

Using the provided `Quat` and `RotorCodebook`:
1. Implement `Quat TreeRotorOperator::fold_dag(const std::vector<ExprNode>& nodes, uint32_t root_id, size_t* out_visited_count)` using a memo table `std::unordered_map<uint32_t, Quat> memo` so each unique DAG node ID is evaluated at most once.
2. For a node `node` with `base_anchor = (k_idx < 11) ? codebook_.constr_rotors[k_idx] : Quat::identity()` (where `p.rotate(r) = r * p * conj(r)`):
   - **`BVAR`, `FVAR`, `MVAR`, `SORT`:**
     $$\theta = (\text{payload} + 1) \cdot 0.314159, \qquad \text{res} = \text{base\_anchor}.\text{rotate}(\text{make\_rotor}(\theta, 0.577, 0.577, 0.577))$$
   - **`CONST`:**
     Let $h = \text{fnv1a\_64}(\text{symbol})$ and $\theta = (h \bmod 1000) \cdot \frac{6.2831853}{1000.0}$:
     $$\text{res} = \text{base\_anchor}.\text{rotate}(\text{make\_rotor}(\theta, 0.8, 0.6, 0.0))$$
   - **`APP`:**
     If `children.size() >= 2`, fold `q_fn = fold(children[0])` and `q_arg = fold(children[1])`, apply asymmetric branch rotors `rot_fn = q_fn.rotate(codebook_.r_left)` and `rot_arg = q_arg.rotate(codebook_.r_right)`, and normalize their component-wise quaternion sum:
     $$\text{res} = (\text{rot\_fn} + \text{rot\_arg}).\text{normalize}()$$
   - **`LAM`, `FORALLE`:**
     If `children.size() >= 2`, fold `q_dom` and `q_body`, rotate by `r_dom` and `r_body`, and normalize their sum.
   - **`LETE`:**
     If `children.size() >= 3`, fold `children[0..2]`, rotate by `r_let_type`, `r_let_val`, `r_let_body`, and normalize their sum.
   - **`LIT`:**
     $$\theta = (\text{payload} \bmod 500) \cdot \frac{3.14159}{250.0}, \qquad \text{res} = \text{base\_anchor}.\text{rotate}(\text{make\_rotor}(\theta, 0.0, 0.707, 0.707))$$
   - **`MDATA`:**
     If `!children.empty()`, $\text{res} = \text{fold}(\text{children}[0]).\text{rotate}(\text{base\_anchor})$.

## Mathematical Law / Invariant

- **Unit Sphere Closure:** For every non-empty DAG, $\|\text{fold\_dag}(\text{nodes}, \text{root})\|_2 = 1.0$.
- **Non-Commutative Asymmetry:** Because $R_{\text{left}} \neq R_{\text{right}}$ in $SU(2)$, for distinct subtrees $a \neq b$:
  $$d_{S^3}\big(\text{fold}(\text{APP}(a, b)),\ \text{fold}(\text{APP}(b, a))\big) > 0$$
- **Zero Window Truncation & $O(|V|)$ DAG Memoization:** Every reachable node in a 63-node depth-6 tree is visited (`visited == 63`), and shared diamond subgraphs are evaluated only once.

## Function Signature

```cpp
Quat TreeRotorOperator::fold_dag(
    const std::vector<ExprNode>& nodes,
    uint32_t root_id,
    size_t* out_visited_count = nullptr
) const;
```

## Complexity Target

- **Time:** $O(|V|)$ quaternion rotor products with memoization.
- **Space:** $O(|V|)$ memoization map.

## Source Reference

- [tree_rotor.cpp](file:///home/yassine/code/r4-prover/src/cpp/geometry/tree_rotor.cpp#L17-L158)
- [tree_rotor.hpp](file:///home/yassine/code/r4-prover/src/cpp/geometry/tree_rotor.hpp)
