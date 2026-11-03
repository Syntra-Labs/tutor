# 059: $E_8$ 256-Cell Product-Address Shortlist Admission Gate

## Problem Statement

To route queries across tens of thousands of memory cells without exhaustive scanning, the manifold partitions cells into $16 \times 16 = 256$ product-address buckets (`admission_cell = (r_0 << 4) | r_3`), where $r_0, r_3 \in [0, 15]$ are the snapped 16-root orbit indices of the first and last 8D blocks of an 8192-bit hypervector.

Given a query's `E8CarrierAddress` (`first_block[8]` and `last_block[8]`) and the 16 canonical root vectors `E8_ROOTS[16][8]`:
1. Compute the $L_1$ distance cost from `first_block` to each root $r \in [0, 15]$:
   $$\text{first\_cost}[r] = \sum_{d=0}^{7} |\text{first\_block}[d] - \text{E8\_ROOTS}[r][d]|$$
   and similarly $\text{last\_cost}[r]$ from `last_block`.
2. Form the 256 product-cell costs:
   $$\text{cell\_l1\_cost}[c] = \text{first\_cost}[c \gg 4] + \text{last\_cost}[c \ \& \ 15] \quad \forall c \in [0, 255]$$
3. Rank the 256 cells by $(\text{cell\_l1\_cost}[c], c)$ ascending (breaking ties by smaller cell index $c$) and return a boolean mask `std::array<bool, 256>` with `true` for the top $K = 32$ admitted cells and `false` for the remaining 224 cells.

## Mathematical Law / Invariant

- **Exact Cardinality:** Exactly $K = 32$ entries in the returned mask are `true`: $\sum_{c=0}^{255} \mathbb{I}[\text{admitted}[c]] = 32$.
- **Home Cell Admission:** If `first_block` equals root $r_0$ and `last_block` equals root $r_3$, then the home product cell $c^* = (r_0 \ll 4) \mid r_3$ has $\text{cell\_l1\_cost}[c^*] = 0$ and is guaranteed to be admitted ($\text{admitted}[c^*] = \text{true}$).
- **Monotonicity & Tie-Breaking:** For any admitted cell $a$ and rejected cell $b$, either $\text{cell\_l1\_cost}[a] < \text{cell\_l1\_cost}[b]$ or ($\text{cell\_l1\_cost}[a] == \text{cell\_l1\_cost}[b]$ and $a < b$).

## Function Signature

```cpp
std::array<bool, 256> compute_e8_shortlist_admission(
    const E8CarrierAddress& query_addr,
    std::array<uint32_t, 256>* out_cell_costs = nullptr
);
```

## Complexity Target

- **Time:** $O(256 \log 32)$ comparisons via `std::partial_sort` or radix bucket selection (< 500 cycles/op).
- **Space:** $O(1)$ stack memory.

## Source Reference

- [manifold.cpp](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L2053-L2073)
- [admission.md](file:///home/yassine/code/uor-r4/docs/mechanisms/admission.md)
