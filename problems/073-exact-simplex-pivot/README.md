# 073: Exact Rational Simplex Basis Pivot & Certificate Verifier

## Problem Statement

Floating-point LP solvers suffer from round-off drift and basis cycling on degenerate polytopes. The exact legal-construction solver (`docs/labs/legal-construction-exact-solve/solver/src/lp.rs`) runs primal simplex with **Bland's anti-cycling rule** over exact reduced rationals $\mathbb{Q}$ (`Rational{num, den}`) with **zero floating-point operations**.

Given an exact `Rational` type (`int64_t num, den` kept reduced by $\gcd$ with $\text{den} > 0$) and a `SimplexTableau`:
1. **`void SimplexTableau::pivot(size_t r, size_t j)`:**
   - Divide row `r` (`t[r]` and `rhs[r]`) by the pivot element $p = t[r][j]$.
   - For every other row $i \neq r$ with $f = t[i][j] \neq 0$, subtract $f \cdot \text{row}_r$ from row $i$ (`t[i]` and `rhs[i]`).
   - For the reduced-cost vector `d`, let $f_d = d[j]$; subtract $f_d \cdot \text{row}_r$ from `d`.
   - Set `basis[r] = j`.
2. **`bool SimplexTableau::step_blands_rule(size_t allowed_cols)`:**
   - **Entering column $j$:** Select the **smallest** column index $j \in [0, \text{allowed\_cols})$ with strictly negative reduced cost $d[j] < 0$. If none exists, the current basis is optimal (return `false` = no pivot needed).
   - **Leaving row $r$:** Among all rows $i \in [0, m)$ with $t[i][j] > 0$, minimize the ratio $\frac{\text{rhs}[i]}{t[i][j]}$, breaking ties by choosing the row $i$ with the **smallest basic variable index** `basis[i]` (Bland's rule).
   - Perform `pivot(r, j)` and return `true`.
3. **`bool verify_lp_certificate(A, b, c, x_primal, y_dual)`:**
   - Verify exact primal-dual optimality for $\min c^T x \text{ s.t. } Ax \le b, x \ge 0$ and its dual $\max b^T y \text{ s.t. } A^T y \le c, y \le 0$ (or for equality form $Ax = b, x \ge 0$ and $A^T y \le c$):
     - Here we use standard equality form $\min c^T x \text{ s.t. } Ax = b, x \ge 0$:
       1. **Primal feasibility:** $x_j \ge 0 \ \forall j$ and $\sum_j A_{ij} x_j = b_i \ \forall i$.
       2. **Dual feasibility:** $\sum_i A_{ij} y_i \le c_j \ \forall j$.
       3. **Strong duality (zero duality gap):** $\sum_j c_j x_j = \sum_i b_i y_i$.

## Mathematical Law / Invariant

- **Bland's Anti-Cycling Theorem:** Choosing the smallest entering index $j$ with $d_j < 0$ and breaking leaving-row ratio ties by smallest `basis[i]` guarantees finite termination without cycling on degenerate pivots.
- **Strong Duality Certificate:** Any pair $(x^*, y^*)$ satisfying $Ax^* = b, x^* \ge 0$, $A^T y^* \le c$, and $c^T x^* = b^T y^*$ is a machine-checkable proof of global optimality.

## Function Signature

```cpp
void SimplexTableau::pivot(size_t r, size_t j);
bool SimplexTableau::step_blands_rule(size_t allowed_cols);
bool verify_lp_certificate(
    const std::vector<std::vector<Rational>>& A,
    const std::vector<Rational>& b,
    const std::vector<Rational>& c,
    const std::vector<Rational>& x_primal,
    const std::vector<Rational>& y_dual
);
```

## Complexity Target

- **Time:** $O(m \cdot n)$ exact rational operations per pivot; $O(m \cdot n)$ for certificate verification.
- **Space:** $O(m \cdot n)$ tableau entries.
