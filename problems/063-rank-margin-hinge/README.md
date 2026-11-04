# 063: Integer Rank-Margin Hinge Loss (Objectives A & R)

## Problem Statement

Under the no-softmax learning law (D23) and zero-multiplier serving contract (D11), discrete geometric models train on an exact integer rank-margin hinge loss over token $L_1$ distances $d$ and cell address distances $\text{addr}$.

Given:
- Target token $L_1$ distance $d_{\text{target}}$ and best wrong rival token $L_1$ distance $d_{\text{rival}}$,
- Target cell address distance $\text{addr}_{\text{target}}$ and best wrong occupied rival cell address distance $\text{addr}_{\text{rival\_cell}}$,
- Array of cell address distances `cell_addr_dist[256]`, boolean cell occupancy mask `cell_occupied[256]`, target cell index `target_cell`, and kept shortlist budget $K$:

Compute:
1. **Token Margin Shortfall ($w = 32$):**
   $$T = \max(0,\ 32 + d_{\text{target}} - d_{\text{rival}})$$
2. **Address Margin Shortfall:**
   $$A = \max(0,\ 8 + \text{addr}_{\text{target}} - \text{addr}_{\text{rival\_cell}})$$
3. **Objective A Numerator (using bit shifts only):**
   $$\text{loss\_A\_num} = 8T + 32A = (T \ll 3) + (A \ll 5)$$
4. **Occupied Cell Rank $r$ and Objective R Shortfall:**
   Let $r$ be $1$ plus the number of occupied cells $c \in [0, 255]$ ($c \neq \text{target\_cell}$ and `cell_occupied[c] == true`) that strictly precede `target_cell` under the lexicographic order $(\text{cell\_addr\_dist}[c],\ c)$. Then:
   $$R = \max(0,\ r - K)$$
   and Objective R numerator is:
   $$\text{loss\_R\_num} = (T \ll 3) + (R \ll 5)$$

## Mathematical Law / Invariant

- **Zero-Loss Margin:** When $d_{\text{target}} \le d_{\text{rival}} - 32$ and $\text{addr}_{\text{target}} \le \text{addr}_{\text{rival\_cell}} - 8$, both $T = 0$ and $A = 0$, so $\text{loss\_A\_num} = 0$.
- **Admission Equivalence of Objective R:** $R = 0$ if and only if the target cell is among the first $K$ occupied cells in `(cell_addr_dist[c], c)` order ($r \le K$).
- **Multiplier-Free Arithmetic:** All loss combinations use only shifts (`<< 3`, `<< 5`), additions, subtractions, and comparisons.

## Function Signature

```cpp
RankMarginResult evaluate_rank_margin_hinge(
    int32_t d_target,
    int32_t d_best_rival,
    int32_t addr_target,
    int32_t addr_best_rival_cell,
    const std::array<int32_t, 256>& cell_addr_dist,
    const std::array<bool, 256>& cell_occupied,
    uint8_t target_cell,
    int32_t K
);
```

## Complexity Target

- **Time:** $O(256)$ integer comparisons and shifts; zero multiplications or floating-point instructions.
- **Space:** $O(1)$.
