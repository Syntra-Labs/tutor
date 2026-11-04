# 055: Hypervector-to-$E_8$ 256-Cell Product-Address Decoder

## Problem Statement

To route an 8192-bit binary hypervector (`Hypervec8192`, stored as $128 \times \text{uint64\_t}$ words) into the $E_8$ discrete manifold without floating-point projections, `decode_e8_carrier` partitions the 128 words into 4 blocks of 32 words. Each block is further partitioned into 8 coordinates of 4 words ($256$ bits per coordinate), mapped to signed integer coordinates via `(popcount_256 - 128) / 6`, snapped to a 4-bit root index $r_b \in [0, 15]$, and packed into an 8-bit 256-cell admission index `admission_cell = (r_0 << 4) | r_3`.

## Mathematical Law / Invariant

For block $b \in \{0, 1, 2, 3\}$ and coordinate $d \in \{0, \dots, 7\}$:
1. **256-Bit Slice Popcount & Centered Integer Coordinate:**
   Let $\mathrm{base\_w} = 32 b + 4 d$.
   $$\mathrm{ones}_{b,d} = \sum_{k=0}^3 \mathrm{popcount}(\mathrm{hv.words}[\mathrm{base\_w} + k]) \in [0, 256]$$
   $$\mathrm{coords}_{b,d} = \frac{\mathrm{int32}(\mathrm{ones}_{b,d}) - 128}{6} \quad \text{(truncated toward zero)}$$
2. **Block Root Snapping & Product Cell Packing:**
   - Store $\mathrm{first\_block}[d] = \mathrm{coords}_{0,d}$ and $\mathrm{last\_block}[d] = \mathrm{coords}_{3,d}$.
   - $\mathrm{block\_roots}[b] = \mathrm{snap\_e8\_root}(\mathrm{roots}, \mathrm{coords}_b, 1) \in [0, 15]$.
   - $\mathrm{admission\_cell} = (\mathrm{block\_roots}[0] \ll 4) \mid \mathrm{block\_roots}[3] \in [0, 255]$.

## Function Signature

```cpp
E8CarrierAddress decode_e8_carrier(
    const Hypervec8192& hv,
    const std::array<std::array<int8_t, 8>, 16>& roots = CANONICAL_E8_ORBIT_16
);
```

## Complexity Target

- **Time:** $O(1)$ (128 `std::popcount` calls + 4 root snaps).
- **Space:** $O(1)$.
