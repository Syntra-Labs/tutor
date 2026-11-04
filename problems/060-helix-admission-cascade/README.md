# 060: Two-Stage Helix Signature + Admission Pre-Filter Cascade

## Problem Statement

Before touching 1 KB `Hypervec8192` payloads in DRAM, the manifold scans a contiguous `HotRouteEntry` array in L2 cache (36 bytes per cell: `HaloHeader` + 32-byte `HelixSignature256`).

Given a query signature `q_sig`, the 256-cell boolean admission mask `is_admitted_cell`, and a span of `HotRouteEntry` records:
1. **Stage 1 (Metadata & Coarse Gate):**
   - Skip any cell where `halo.rejected == true`.
   - Compute 64-bit coarse signature Hamming distance: `sig_ham = popcount(q_sig.s0 ^ cell.sig.s0)`.
   - Look up `admitted = is_admitted_cell[cell.halo.sector_id]`.
   - If `!admitted && sig_ham > 28`, reject the cell immediately.
2. **Stage 2 (Combined Coarse Cost & Shortlist Selection):**
   - Compute 192-bit residual signature Hamming distance:
     $$\text{raw192\_ham} = \text{popcount}(q.\text{s1} \oplus c.\text{s1}) + \text{popcount}(q.\text{s2} \oplus c.\text{s2}) + \text{popcount}(q.\text{s3} \oplus c.\text{s3})$$
   - Compute the combined integer cost:
     $$\text{combined\_cost} = (\text{sig\_ham} \ll 2) + \text{raw192\_ham} + (\text{admitted} \ ? \ 0 : 6)$$
   - Reject if `combined_cost > 212`.
   - Return the top $\min(N_{\text{passed}}, 144)$ candidates ordered by `(coarse_cost, cell_idx)` ascending.

## Mathematical Law / Invariant

- **Non-Admitted Early Rejection:** Any cell with `is_admitted_cell[sector_id] == false` and `popcount(q_sig.s0 ^ c_sig.s0) > 28` is strictly excluded from the shortlist.
- **Admission Bonus:** An admitted cell receives a $+0$ penalty whereas a non-admitted cell with identical `HelixSignature256` receives $+6$ penalty in `combined_cost`.
- **Shortlist Cap & Ordering:** The returned vector has size $\le 144$ and is sorted monotonically by `(coarse_cost, cell_idx)` ascending.

## Function Signature

```cpp
std::vector<ShortlistCand> run_helix_admission_cascade(
    const HelixSignature256& q_sig,
    const std::array<bool, 256>& is_admitted_cell,
    const std::vector<HotRouteEntry>& hot_route,
    size_t max_verify = 144,
    uint16_t cutoff_cost = 212
);
```

## Complexity Target

- **Time:** $O(N)$ linear scan with hardware `popcnt` + $O(M \log K)$ selection.
- **Space:** $O(M)$ candidate buffer.
