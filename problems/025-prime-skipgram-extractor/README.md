# 025: CamelCase, N-Gram & Prime-Stride Skip-Gram Extractor

## Problem Statement

To encode source code and natural text into hypervectors without a vocabulary lookup table or heap allocations, `extract_patches` scans a byte span (up to 1536 bytes) and extracts up to 120 `ExtractedPatch` items on the stack:

1. **CamelCase & Identifier Splitting:**
   - Uppercase `'A'..'Z'` following a lowercase `'a'..'z'` flushes the current word before appending `tolower(c)`.
   - Lowercase `'a'..'z'`, digits `'0'..'9'`, and non-ASCII bytes `>= 0x80` append to `cur_word` (up to 63 chars).
   - Any other byte flushes `cur_word`.
2. **Word Patch Emission (`flush_word` when `cur_len >= 2` and `n_patches < 120`):**
   - Primary token: `{fnv1a_64(cur_word, cur_len), base_w, true}` where `base_w = (cur_len >= 5) ? 4 : ((cur_len >= 3) ? 3 : 2)`.
   - If `cur_len >= 5`: for `j = 0; j + 4 <= cur_len && j < 12 && n_patches < 120; j += 2`, emit 4-gram `{fnv1a_64(cur_word + j, 4, 0x9E3779B97F4A7C15ULL), 2, false}`.
   - If `cur_len >= 4` and `n_patches < 120`: emit 3-char prefix `{fnv1a_64(cur_word, 3, 0xBF58476D1CE4E5B9ULL), 1, false}`.
3. **Non-Commutative Prime-Stride Bigram & Skip-Gram Seeds:**
   - For consecutive primary token seeds $s_{t-2}, s_{t-1}, s_t$:
     - Order-preserving bigram (`PRIME_STRIDE_0 = 3`):
       $$\text{bg\_seed}(s_{t-1}, s_t) = \text{rotl}(s_{t-1}, 3) \oplus (s_t \cdot \text{0x9E3779B97F4A7C15ULL})$$
     - Order-preserving 1-skip-gram (`PRIME_STRIDE_1 = 5`):
       $$\text{sg\_seed}(s_{t-2}, s_t) = \text{rotl}(s_{t-2}, 5) \oplus (s_t \cdot \text{0xBF58476D1CE4E5B9ULL})$$

## Mathematical Law / Invariant

- **Non-Commutativity of Prime-Stride Binding:** For distinct token seeds $A \neq B$, $\text{bg\_seed}(A, B) \neq \text{bg\_seed}(B, A)$, preserving syntactic token order `(a, b)` vs `(b, a)` without positional embeddings.
- **CamelCase Decomposition:** `"hyperVec"` decomposes into primary tokens `"hyper"` (length 5, weight 4) and `"vec"` (length 3, weight 3).

## Function Signature

```cpp
struct ExtractedPatch {
    uint64_t seed;
    int16_t intrinsic_weight;
    bool is_primary_token;
};

uint64_t fnv1a_64(const char* data, size_t len, uint64_t basis = 0xCBF29CE484222325ULL);
uint64_t make_bigram_seed(uint64_t prev1_primary, uint64_t cur_primary);
uint64_t make_skipgram_seed(uint64_t prev2_primary, uint64_t cur_primary);
size_t extract_patches(std::string_view text, ExtractedPatch out_patches[128]);
```

## Complexity Target

- **Time:** $O(\min(N, 1536))$ single-pass scan with zero heap allocations.
- **Space:** $O(1)$ stack buffers.
