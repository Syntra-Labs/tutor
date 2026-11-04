# 062: 5-Edge `MODE_RELATION` Structural Adjacency Coupler

## Problem Statement

When indexing code blocks into the geometric manifold, each block is wired in $O(1)$ time to 5 structural edge types (`e=0..4`) and evaluated during routing via fast 256-bit `HelixSignature256` resonance:
- **Edge `e=0` (Folded 3-Block Triad Consensus):** Every 3 consecutive blocks `(c0, c1, c2)` fold their `HelixSignature256` via bitwise majority `(s0 & s1) | (s1 & s2) | (s0 & s2)` into `triad_sigs` and record `triad_sig_idx`.
- **Edge `e=1` (Ternary Triad Sibling Coupling):** Within a completed triad `(c0, c1, c2)`, `c0.triad_sib0 = c1`, `c1.triad_sib0 = c0`, `c1.triad_sib1 = c2`, and `c2.triad_sib0 = c1`.
- **Edge `e=2` (Sequential Same-File Adjacency):** If `prev_seq_in_file != ABSENT_NODE`, link `rl[cell_idx].seq_prev_idx = prev_seq_in_file` and `rl[prev_seq_in_file].seq_next_idx = cell_idx`, then update `prev_seq_in_file = cell_idx`.
- **Edge `e=3` (Primary Semantic Mass Twin):** Using a 1024-slot hash table indexed by `slot = (seed ^ (seed >> 23)) & 1023`, if `seed != 0` and the previous occupant at `slot` has the exact same `primary_mass_seed`, link `rl[cell_idx].mass_twin_idx = prev_mass`, then overwrite `online_mass_table[slot] = cell_idx`.
- **Edge `e=4` ($E_8$ Sector Frame Twin):** If `online_frame_table[sector_id] != ABSENT_NODE`, link `rl[cell_idx].frame_twin_idx = online_frame_table[sector_id]`, then update `online_frame_table[sector_id] = cell_idx`.

During routing, `compute_coupled_similarity` combines `raw_sim`, `file_sim`, and a structural bonus `rel_bonus` capped at `0.025`:
$$\text{sim} = 0.85 \cdot \text{raw\_sim} + 0.15 \cdot \text{file\_sim} + \min(0.025,\ \text{rel\_bonus})$$
where for signature similarity $s(\text{sig}) = 1.0 - \frac{\text{hamming}(q_{\text{sig}}, \text{sig})}{256.0}$:
- `e=0`: if $s(\text{triad\_sig}) > 0.52$, add $0.12 \cdot (s - 0.50)$
- `e=1` (`sib0`): if $s(\text{sib0}) > 0.53$, add $0.08 \cdot (s - 0.50)$
- `e=1` (`sib1`): if $s(\text{sib1}) > 0.53$, add $0.06 \cdot (s - 0.50)$
- `e=2` (`seq_prev`): if $s(\text{seq\_prev}) > 0.53$, add $0.07 \cdot (s - 0.50)$
- `e=3` (`mass_twin`): if $s(\text{mass\_twin}) > 0.53$, add $0.06 \cdot (s - 0.50)$
- `e=4` (`frame_twin`): if $s(\text{frame\_twin}) > 0.53$, add $0.05 \cdot (s - 0.50)$

## Mathematical Law / Invariant

- **$O(1)$ Wiring Complexity:** Each call to `wire_block_relations` performs a constant number of table lookups and updates.
- **Bounded Bonus Invariant:** The structural relation bonus never exceeds $+0.025$.

## Function Signature

```cpp
void wire_block_relations(
    uint32_t cell_idx,
    uint32_t& prev_seq_in_file,
    const std::vector<BlockMeta>& cells,
    std::vector<ModeRelationLinks>& rel_links,
    std::vector<HelixSignature256>& triad_sigs,
    RelationIndexerState& state
);

double compute_coupled_similarity(
    uint32_t cell_idx,
    double raw_sim,
    double file_sim,
    const HelixSignature256& q_sig,
    const std::vector<BlockMeta>& cells,
    const std::vector<ModeRelationLinks>& rel_links,
    const std::vector<HelixSignature256>& triad_sigs
);
```

## Complexity Target

- **Time:** $O(1)$ per block for both wiring and coupled similarity evaluation.
- **Space:** $O(1)$ auxiliary state (`RelationIndexerState`).
