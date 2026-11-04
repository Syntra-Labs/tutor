# 023: Online Hash-Bucket Document-Frequency Surprisal Sketch

## Problem Statement

When encoding code or text spans into hypervectors in a single pass, high-frequency boilerplate tokens (`if`, `return`, `const`, `std`) would dominate the superposition unless attenuated by inverse document frequency (IDF) surprisal.

Implement `SurprisalSketch`, a fixed-memory ($2^{16} = 65536$ buckets) online count sketch that tracks how many spans each 64-bit feature `seed` has appeared in, supports both incremental insertion (`delta > 0`) and deletion (`delta < 0`, saturating at 0 for buckets and at 1 for `total_spans`), and computes stepped integer salience weights.

## Mathematical Law / Invariant

1. **Bucket Projection:** Every 64-bit `seed` maps to bucket index:
   $$\text{idx}(\text{seed}) = (\text{seed} \oplus (\text{seed} \gg 17)) \mathbin{\&} 65535$$
2. **Cold-Start Guard:** When `total_spans <= 4`, `weight_for_seed(seed, base_weight)` returns `base_weight` unchanged.
3. **Stepped Inverse Document Frequency Law (for `total_spans > 4`):**
   Let $df = \text{bucket\_counts}[\text{idx}(\text{seed})]$.
   - If $df = 0$: return `base_weight + 2`.
   - Otherwise, let $\text{ratio} = \lfloor \frac{\text{total\_spans} + 1}{df + 1} \rfloor$:
     - $\text{ratio} \le 3 \implies 1$ (ubiquitous boilerplate attenuated to weight 1)
     - $4 \le \text{ratio} \le 8 \implies 2$
     - $9 \le \text{ratio} \le 24 \implies \text{base\_weight}$
     - $\text{ratio} > 24 \implies \text{base\_weight} + 2$ (rare high-surprisal identifier boosted)

## Function Signature

```cpp
struct SurprisalSketch {
    static constexpr size_t SURPRISAL_BUCKETS = 65536;
    std::array<uint32_t, SURPRISAL_BUCKETS> bucket_counts{};
    uint32_t total_spans = 1;

    void clear();
    void observe_seed(uint64_t seed, int32_t delta = 1);
    void step_span(int32_t delta = 1);
    int16_t weight_for_seed(uint64_t seed, int16_t base_weight = 4) const;
};
```

## Complexity Target

- **Time:** $O(1)$ per `observe_seed`, `step_span`, and `weight_for_seed` (zero branches on hash lookup).
- **Space:** Exactly $256\text{ KiB} + 4\text{ B}$ (`65536 * sizeof(uint32_t)`).
