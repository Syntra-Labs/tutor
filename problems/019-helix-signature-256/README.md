# 019: 256-Bit Multi-Resolution Helix Quadrant Signature

## Problem Statement

Scanning thousands of full 8,192-bit (1,024-byte) hypervectors during first-stage candidate filtering is memory-bandwidth bound. A **256-bit Multi-Resolution Helix Signature** (`HelixSignature256`, 32 bytes) fits two signatures per 64-byte cache line while combining coarse global coverage with fine-grained prefix bits:
- **`s0` (Coarse Global Summary):** Each bit $b \in [0, 63]$ summarizes the 128-bit pair `(hv.words[2*b], hv.words[2*b + 1])`. Let `ones = popcount(words[2*b]) + popcount(words[2*b + 1])`. Bit $b$ of `s0` is `1` iff `ones > 64 || (ones == 64 && (b & 1))`.
- **`s1, s2, s3` (Fine Prefix Slices):** Direct copies of `hv.words[0]`, `hv.words[1]`, and `hv.words[2]`.

Implement `compute_helix_signature(const Hypervec8192& hv)` and `HelixSignature256::hamming(const HelixSignature256& other) const`.

## Mathematical Law / Invariant

- **Unbiased Coarse Compression:** A 128-bit slice has an even number of bits, so ties (`ones == 64`) occur with probability $\binom{128}{64} 2^{-128} \approx 7.04\%$. Breaking ties via `(b & 1)` preserves exact 50% bit balance across `s0` on neutral slices (`ones == 64` everywhere $\implies \text{s0} = \text{0xAAAAAAAAAAAAAAAAULL}$).
- **4-Instruction Metric Filter:** Computing Hamming distance on `HelixSignature256` requires just 4 XOR + `popcnt` instructions ($32\times$ fewer words than `Hypervec8192`).

## Function Signature

```cpp
struct HelixSignature256 {
    uint64_t s0 = 0;
    uint64_t s1 = 0;
    uint64_t s2 = 0;
    uint64_t s3 = 0;

    uint32_t hamming(const HelixSignature256& other) const;
};

HelixSignature256 compute_helix_signature(const Hypervec8192& hv);
```

## Complexity Target

- **Time:** $O(128)$ words to build `s0`; $O(1)$ (4 `popcnt` instructions) for `HelixSignature256::hamming`.
- **Space:** 32 bytes per signature.

## Source

- [`manifold.cpp` (lines 487–509)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L487-L509)
