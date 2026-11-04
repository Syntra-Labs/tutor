# 026: 90% Salience Sparsification (Top-K Semantic Mass Nodes)

## Problem Statement

Bundling all extracted n-grams and shingles into an 8192-bit hypervector degrades signal-to-noise ratio (SNR) and increases inner-loop work. Instead, the **90% Salience Sparsification** pass deduplicates feature seeds, accumulates repeat bonuses, identifies the single highest-weight primary token seed (`primary_mass_seed`) for cross-file twin routing (`e = 3`), and retains only the top $K$ (`max_mass_nodes`, default 20) Semantic Mass Nodes via `std::partial_sort`.

## Mathematical Law / Invariant

Given an input list of `ExtractedPatch` items `(seed, intrinsic_weight, is_primary_token)`:

1. **Deduplication & Repeat Reinforcement:**
   - If `seed` has not been seen yet (up to 192 unique entries), insert `(seed, intrinsic_weight)` with `seed_is_primary = is_primary_token`.
   - If `seed` is already present at index `i`, increment its weight by $\max(1, \lfloor \text{intrinsic\_weight} / 2 \rfloor)$.
2. **Primary Mass Seed Selection:**
   - Among all unique seeds with `seed_is_primary[i] == true`, select the first seed that maximizes `weight` (strict `>` comparison: `weight > best_w`).
3. **Priority Salience Score & Top-$K$ Truncation:**
   - Each unique feature $i$ is ranked by salience score:
     $$\text{score}(i) = \text{weight}_i + \begin{cases} 2 & \text{if } \text{seed\_is\_primary}[i] \\ 0 & \text{otherwise} \end{cases}$$
   - If `num_unique > max_mass_nodes`, `std::partial_sort` retains the top `max_mass_nodes` features in descending order of $\text{score}(i)$ (breaking ties by smaller original unique index `i` for determinism).

## Function Signature

```cpp
struct ExtractedPatch {
    uint64_t seed;
    int16_t intrinsic_weight;
    bool is_primary_token;
};

struct SparsifiedMassResult {
    std::vector<std::pair<uint64_t, int16_t>> retained_nodes;
    uint64_t primary_mass_seed = 0;
};

SparsifiedMassResult sparsify_mass_nodes(
    const ExtractedPatch* patches,
    size_t n_patches,
    size_t max_mass_nodes = 20
);
```

## Complexity Target

- **Time:** $O(U \log K)$ where $U \le 192$ is the number of unique seeds and $K \le 20$.
- **Space:** $O(U)$ stack arrays.
