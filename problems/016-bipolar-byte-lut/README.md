# 016: Precomputed `256x8` Bipolar Byte Expansion LUT

## Problem Statement

Unpacking individual bits of a hypervector into signed bipolar integers $\{-1, +1\} \subset \text{int16\_t}$ via serial bit-shifts (`((w >> k) & 1) ? 1 : -1`) creates a 64-iteration loop per word. By precomputing a `constexpr` 32-byte-aligned lookup table `BipolarByteLUT` of size $256 \times 8 \times \text{sizeof(int16\_t)} = 4096$ bytes (fitting inside L1D cache), each byte expands in a single 128-bit load (`vmovdqa`).

Implement:
1. `BipolarByteLUT`: `constexpr` constructor populating `table[b][k] = ((b >> k) & 1) ? +1 : -1` for $b \in [0, 255]$ and $k \in [0, 7]$, with `alignas(32)` on `table`.
2. `void expand_word_bipolar(uint64_t word, int16_t* out_64)`:
   - Expand all 8 bytes of `word` (little-endian bit order $0 \dots 63$) into `out_64[0..63]` using `BYTE_BIPOLAR_LUT.table[static_cast<uint8_t>(word >> (8 * byte_idx))]`.

## Mathematical Law / Invariant

- **Isomorphism $(\{0,1\}, \oplus) \cong (\{+1,-1\}, \times)$:** Under the map $\phi(b) = 2b - 1 \in \{-1, +1\}$:
  $$\phi(x \oplus 1) = -\phi(x), \quad \sum_{k=0}^{7} \text{table}[b][k] = 2\,\operatorname{popcount}(b) - 8$$
- **Antipodal Byte Symmetry:** For every byte $b \in [0, 255]$ and bit $k \in [0, 7]$:
  $$\text{table}[b][k] + \text{table}[255 - b][k] = 0$$

## Function Signature

```cpp
struct BipolarByteLUT {
    alignas(32) int16_t table[256][8]{};
    constexpr BipolarByteLUT();
};

static constexpr BipolarByteLUT BYTE_BIPOLAR_LUT{};

void expand_word_bipolar(uint64_t word, int16_t* out_64);
```

## Complexity Target

- **Time:** $O(1)$ — 8 L1D cache-hit 16-byte loads/stores per 64-bit word.
- **Space:** 4,096 bytes static `constexpr` table in `.rodata`.

## Source

- [`manifold.cpp` (lines 412–423)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L412-L423)
