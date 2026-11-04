# 017: Bipolar Majority-Vote Accumulator & Unrolled Projection

## Problem Statement

In Hyperdimensional Computing, **bundling** (superposition) combines weighted hypervectors $V_1, \dots, V_m$ into a single 8,192-bit prototype that remains similar to all constituents.

Implement `BipolarAccumulator`:
1. `void accumulate(const Hypervec8192& hv, int16_t weight = 1)`:
   - Using `BYTE_BIPOLAR_LUT`, add `weight` to `counts[bit]` where `hv` has bit `1`, and subtract `weight` where `hv` has bit `0`.
   - Update `total_items`: if `weight > 0`, add `weight`; if `weight < 0`, subtract `min(total_items, -weight)`.
2. `Hypervec8192 project() const`:
   - Project `counts[0..8191]` back to an 8,192-bit binary hypervector using **alternating tie-breaking**:
     - Even bit indices ($2k$): bit is `1` iff `counts[2k] > 0`.
     - Odd bit indices ($2k+1$): bit is `1` iff `counts[2k+1] >= 0`.

## Mathematical Law / Invariant

- **Zero-Bias Norm Preservation:** When a bit coordinate has a tied sum (`counts[i] == 0`), a uniform threshold (`> 0` or `>= 0`) would bias the projected hypervector toward all-`0` or all-`1`. Alternating `> 0` on even bits and `>= 0` on odd bits guarantees that an all-zero accumulator projects to `0xAAAAAAAAAAAAAAAAULL` in every word (exact popcount $4096 = \frac{8192}{2}$).
- **Superposition Capacity:** Bundling $M$ random hypervectors yields expected similarity $z \approx \frac{90.51}{\sqrt{M}}$ to each constituent vector.
- **Exact Cancellation (Unbundling):** Accumulating $(V, +w)$ followed by $(V, -w)$ restores `counts` to its exact prior state.

## Function Signature

```cpp
struct BipolarAccumulator {
    alignas(64) std::array<int16_t, 8192> counts{};
    uint32_t total_items = 0;

    void clear();
    void accumulate(const Hypervec8192& hv, int16_t weight = 1);
    Hypervec8192 project() const;
};
```

## Complexity Target

- **Time:** $O(1024)$ bytes via 16-byte LUT SIMD `vpaddw` and 8-way unrolled projection (~350 cycles per hypervector).
- **Space:** 16,384 bytes (`8192 x int16_t`, L1D-resident).
