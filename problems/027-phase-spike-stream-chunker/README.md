# 027: Grammar-Free Byte-Markov + CA 184/30 Stream Chunker

## Problem Statement

When ingesting unformatted, newline-free byte streams (raw binary protocols, DNA sequences, hex dumps, or minified blobs), syntax delimiters like `\n\n` or `{}` are absent.

Implement `chunk_unformatted_byte_stream`, which segments any `std::string_view content` into contiguous `StreamChunk { uint32_t start_byte, end_byte, chunk_idx; }` blocks by combining:
1. Causal Order-2 Dirichlet byte-Markov surprisal $s_t = -\log_2 P(b_t \mid b_{t-1})$ (or $s_t = 4.0$ if `markov == nullptr`), smoothed via an exponential moving average (EMA):
   $$\bar{s}_t = 0.92 \bar{s}_{t-1} + 0.08 s_t, \quad \bar{s}_0 = 4.0$$
2. 64-bit SWAR Cellular Automata Rule 184 / Rule 30 counterfactual match-inversion demarcation count `count_demarcs(window64)` over the 8-byte window `content[pos - 8 .. pos]`.

## Mathematical Law / Invariant

Scanning `pos` at 16-byte aligned steps `for (size_t pos = 64; pos < n; pos += 16)`:
- Let `span_len = pos - start_b`. If `span_len < 96`, `continue` without updating $\bar{s}_t$.
- Update $\bar{s}_t = 0.92 \bar{s}_{t-1} + 0.08 s_t$ using `prev_b = content[pos - 1]` and `cur_b = content[pos]`.
- Load 8 bytes `content[pos - 8 .. pos]` via `std::memcpy(&window64, content.data() + pos - 8, 8)` and evaluate `demarcs = count_demarcs(window64)`.
- Emit a chunk boundary `[start_b, pos)` and advance `start_b = pos, chunk_idx++` whenever either condition holds:
  1. **Joint Phase Spike:** $s_t > \bar{s}_t + 1.8 \;\land\; \text{demarcs} \ge 28 \;\land\; \text{span\_len} \ge 128$
  2. **Horizon Cap:** $\text{span\_len} \ge 512$
- After the loop, if `start_b < n`, emit the trailing chunk `[start_b, n)`.

**Partition Invariant:** The emitted chunks form an exact, gap-free, non-overlapping partition of `[0, content.size())`.

## Function Signature

```cpp
struct StreamChunk {
    uint32_t start_byte;
    uint32_t end_byte;
    uint32_t chunk_idx;
};

uint32_t count_demarcs(uint64_t seed_bits);
std::vector<StreamChunk> chunk_unformatted_byte_stream(
    std::string_view content,
    const ByteMarkovModel* markov = nullptr
);
```

## Complexity Target

- **Time:** $O(N / 16)$ SWAR evaluations (1 evaluation per 16 bytes).
- **Space:** $O(\text{number of chunks})$.
