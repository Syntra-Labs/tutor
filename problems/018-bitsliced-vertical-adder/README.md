# 018: 8-Register Bit-Sliced Carry-Save Hypervector Encoder

## Problem Statement

When bundling $n \le 64$ weighted seed hypervectors (with positive integer weights $w_i \in [1, 15]$ and total weight $W_{\text{tot}} = \sum_i w_i \le 255$), we can eliminate the 16 KB `BipolarAccumulator` array entirely by maintaining **64 parallel 8-bit counters vertically across 8 general-purpose `uint64_t` registers (`b0..b7`)**.

Implement `Hypervec8192 encode_seeds_bitsliced(const std::pair<uint64_t, int16_t>* seeds, const size_t* order, size_t n)`:
1. If `n == 0`, return an all-zero `Hypervec8192`.
2. Clamp each seed's weight to $w_i = \text{clamp}(\text{weight}, 1, 15)$ and sum $W_{\text{tot}} = \sum_{i=0}^{n-1} w_i$.
3. Compute the unsigned majority thresholds:
   - Even bits ($0\text{x}5555555555555555$): $t_{\text{even}} = \lfloor W_{\text{tot}} / 2 \rfloor + 1$, pre-bias $\text{init}_{\text{even}} = 128 - t_{\text{even}}$.
   - Odd bits ($0\text{xAAAAAAAAAAAAAAAA}$): $t_{\text{odd}} = \lfloor (W_{\text{tot}} + 1) / 2 \rfloor$, pre-bias $\text{init}_{\text{odd}} = 128 - t_{\text{odd}}$.
4. Pre-load initial bit-planes `ib[0..7]` from `init_even` (on `0x5555555555555555ULL`) and `init_odd` (on `0xAAAAAAAAAAAAAAAAULL`).
5. For each word $w \in [0, 127]$:
   - Initialize `b0..b7 = ib[0..7]`.
   - For each seed $i \in [0, n-1]$, generate `uint64_t word = splitmix64(states[i])` and ripple-add `word * w_i` (via 4 bit-masks `mask0..mask3` for the 4 bits of $w_i \in [1, 15]$) into `(b0..b7)`.
   - Write `out.words[w] = b7` (the MSB register directly holds the majority-voted 64-bit word!).

## Mathematical Law / Invariant

- **Unsigned-to-Bipolar Threshold Equivalence:** Let $U_j = \sum_{i:\,\text{bit}_j=1} w_i$ be the unsigned weight sum at bit $j$. Since the bipolar sum is $S_j = 2U_j - W_{\text{tot}}$:
  $$S_j > 0 \iff U_j \ge \lfloor W_{\text{tot}}/2 \rfloor + 1 = t_{\text{even}}, \qquad S_j \ge 0 \iff U_j \ge \lfloor (W_{\text{tot}}+1)/2 \rfloor = t_{\text{odd}}$$
- **Pre-Biased MSB Readout:** Initializing the 8-bit vertical counter to $128 - t_j$ shifts the threshold to $128 = 2^7$:
  $$U_j \ge t_j \iff U_j + (128 - t_j) \ge 128 \iff b_7[j] = 1$$
  Hence `out.words[w]` is bit-for-bit identical to expanding every seed and projecting via `BipolarAccumulator`!

## Function Signature

```cpp
Hypervec8192 encode_seeds_bitsliced(
    const std::pair<uint64_t, int16_t>* seeds,
    const size_t* order,
    size_t n
);
```

## Complexity Target

- **Time:** $O(128 \times n)$ — 21 bitwise register ops per seed-word, zero L1D memory traffic inside the inner loop.
- **Space:** $O(1)$ — 8 CPU general-purpose registers (`b0..b7`).

## Source

- [`manifold.cpp` (lines 544–624)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L544-L624)
