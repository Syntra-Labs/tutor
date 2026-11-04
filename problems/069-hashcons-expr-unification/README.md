# 069: Hash-Consed `Lean.Expr` DAG & First-Order Unification Gate

## Problem Statement

Before any neural or geometric operator scores candidate proof tactics, the structural AST Admissibility Gate (`src/ir/admissibility.py` & `src/cpp/ir/kernel_expr.hpp`) interns all expressions over the 11 Lean 4 kernel constructors (`BVAR..PROJ`) into a single **hash-consed DAG** and runs first-order pattern unification to filter admissible moves (`exact`, `intro`, `apply`, `rw`).

Implement:
1. **`int HashConsDAG::intern(ExprKind k, const std::string& label, const std::vector<int>& children)`:**
   - Deduplicate nodes so structurally identical subterms share the exact same integer node ID $u \in [0, N)$. Thus structural equality of two closed subterms is $O(1)$ integer equality `u == v`.
2. **`bool match_expr(const HashConsDAG& dag, int pattern, int target, int depth, int local, std::unordered_map<int, int>& bindings)`:**
   - First-order match of `pattern` (inside `depth` stripped outer `FORALLE` binders and `local` inner `LAM`/`FORALLE` binders) against `target`:
     - If `pattern` is `BVAR` with de Bruijn index $i = \text{stoi}(\text{label})$:
       - If $i < \text{local}$, `target` must also be `BVAR` with index $i$.
       - Otherwise let $\text{abs\_idx} = \text{depth} - 1 - (i - \text{local})$. If $\text{abs\_idx} < 0$, return `false`. If `abs_idx` is already in `bindings`, require `bindings[abs_idx] == target`; else bind `bindings[abs_idx] = target` and return `true`.
     - If `pattern == target` and `depth == 0`, return `true`.
     - Otherwise require `pn.k == tn.k`, `pn.label == tn.label`, and `pn.children.size() == tn.children.size()`, and recursively match all children (incrementing `local` by 1 when descending into child `1` of a `LAM` or `FORALLE` node).
3. **`std::vector<Move> admissible_moves(const HashConsDAG& dag, const std::vector<Hyp>& hyps, int target, const std::vector<std::pair<std::string, int>>& lemmas)`:**
   - Emit `Move{"intro", ""}` if `target` has `k == FORALLE` or its app-spine head is `CONST "Not"`.
   - For each hypothesis `h` (`is_prop == true`):
     - Emit `Move{"exact", h.fv}` if `h.type == target` (O(1) hash-consed pointer equality!).
     - Strip outer `FORALLE`s of `h.type`: if `depth > 0`, conclusion head is rigid (`!= BVAR`), and `match_expr` succeeds against `target`, emit `Move{"apply", h.fv}`.
     - If `h.type` is `@Eq α lhs rhs` (`APP(APP(APP(CONST "Eq", α), lhs), rhs)`), for `(rev=false, lhs)` and `(rev=true, rhs)`: if `side != target` and `side` occurs in `subterms(target)`, emit `Move{"rw", h.fv, rev}`.
   - For each `(name, root)` in `lemmas`:
     - Strip outer `FORALLE`s `(depth, concl)`. If `concl` is rigid:
       - If `match_expr(dag, concl, target, depth, 0, {})`, emit `Move{"apply", name}`.
       - If `concl` is `@Eq α lhs rhs`, for `(rev=false, lhs)` and `(rev=true, rhs)`: if `side` is rigid and `match_expr(dag, side, sub, depth, 0, {})` matches any `sub` in `subterms(target)`, emit `Move{"rw", name, rev}`.

## Mathematical Law / Invariant

- **Hash-Cons Canonical Identity:** Two subtrees constructed independently have the same node ID `id1 == id2` if and only if their constructor trees are isomorphic.
- **Consistent Binder Unification:** A pattern with repeated binder variable `?x` (e.g. `Eq Nat ?x ?x`) matches `Eq Nat a a` (`bindings[0] = a`) but rejects `Eq Nat a b` when `a != b`.

## Function Signature

```cpp
int HashConsDAG::intern(ExprKind k, const std::string& label, const std::vector<int>& children);
bool match_expr(const HashConsDAG& dag, int pattern, int target, int depth, int local, std::unordered_map<int, int>& bindings);
std::vector<Move> admissible_moves(const HashConsDAG& dag, const std::vector<Hyp>& hyps, int target, const std::vector<std::pair<std::string, int>>& lemmas);
```

## Complexity Target

- **Time:** $O(1)$ amortized per `intern`; $O(|P|)$ per unification check.
- **Space:** $O(N_{\text{unique}})$ DAG nodes.
