# 024: Order-2 Interpolated Dirichlet Byte-Markov Surprisal

## Problem Statement

To detect phase boundaries in raw, grammar-free byte streams (minified blobs, binary protocols, DNA sequences), `ByteMarkovModel` maintains an L2-cache-resident $256 \times 256$ bigram count matrix and a 256-entry unigram count vector, and evaluates the causal per-byte surprisal $s_t = -\log_2 P(b_t \mid b_{t-1})$ in bits.

When training on `text` via `observe_stream(text)`:
- If `text.empty()`, return immediately.
- Compute stride `step = (text.size() > 4096) ? (text.size() / 4096) : 1`.
- Observe `prev = static_cast<uint8_t>(text[0])`: increment `unigram[prev]` and `total_bytes`.
- For `i = step; i < text.size(); i += step`: let `cur = static_cast<uint8_t>(text[i])`, increment `bigram[prev][cur]`, `unigram[cur]`, and `total_bytes`, and set `prev = cur`.

## Mathematical Law / Invariant

With symmetric Dirichlet prior $\alpha = 0.4$ over the 256-byte alphabet ($256 \alpha = 102.4$) and Jelinek-Mercer interpolation weights $(\lambda_2, \lambda_1) = (0.76, 0.24)$:

$$P(b_t \mid b_{t-1}) = 0.76 \cdot \frac{C(b_{t-1}, b_t) + 0.4}{C(b_{t-1}) + 102.4} + 0.24 \cdot \frac{C(b_t) + 0.4}{N + 102.4}$$

$$s_t(b_{t-1}, b_t) = -\log_2 \max\!\left(P(b_t \mid b_{t-1}),\, 10^{-12}\right)$$

- **Proper Probability Distribution:** For any fixed conditioning byte $b_{t-1}$ (where $C(b_{t-1}) = \sum_{b} C(b_{t-1}, b)$ and $N = \sum_b C(b)$):
  $$\sum_{b=0}^{255} P(b \mid b_{t-1}) = 0.76 \cdot \frac{C(b_{t-1}) + 256 \cdot 0.4}{C(b_{t-1}) + 102.4} + 0.24 \cdot \frac{N + 256 \cdot 0.4}{N + 102.4} = 1.0$$
- **Maximum-Entropy Cold Start:** On an empty model ($N = 0, C(\cdot) = 0$), every byte transition has $P = \frac{0.4}{102.4} = \frac{1}{256}$ and exact surprisal $s_t = 8.0$ bits.

## Function Signature

```cpp
struct ByteMarkovModel {
    std::array<std::array<uint32_t, 256>, 256> bigram{};
    std::array<uint32_t, 256> unigram{};
    uint64_t total_bytes = 0;

    void observe_stream(std::string_view text);
    double byte_surprisal(uint8_t prev, uint8_t cur) const;
};
```

## Complexity Target

- **Time:** $O(1)$ per `byte_surprisal` evaluation; $O(\min(N, 4096))$ per `observe_stream`.
- **Space:** $256\text{ KiB} + 1\text{ KiB}$ fixed L2-resident tables.

## Source Reference

- [`manifold.cpp` (lines 731–762)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L731-L762)
