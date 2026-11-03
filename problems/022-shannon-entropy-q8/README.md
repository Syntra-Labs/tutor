# 022: Empirical Byte Shannon Entropy & Q8 Quantizer

## Problem Statement

Given a byte span, compute its empirical Shannon entropy $H \in [0, 8]$ in bits per byte and quantize it into an 8-bit unsigned header field `entropy_q8` for the `HaloHeader` routing metadata.

For spans with `text.size() > 1024`, sample bytes at stride `step = text.size() / 1024` (`step = 1` when `text.size() <= 1024`).

## Mathematical Law / Invariant

Let $c_b$ be the count of byte value $b \in \{0, \dots, 255\}$ among $M$ sampled bytes, and $p_b = \frac{c_b}{M}$:

$$H(X) = -\sum_{\substack{b=0 \\ c_b > 0}}^{255} p_b \log_2 p_b \in [0.0, 8.0]$$

The Q8 fixed-point encoding maps $[0, 8]$ bits/byte into $[0, 255]$:

$$\text{entropy\_q8} = \text{clamp}\!\left(\lfloor 32.0 \cdot H(X) \rfloor, 0, 255\right)$$

- **Zero-entropy lower bound:** A constant byte stream (`"aaaaaaaa"`) has $H = 0.0$ and `entropy_q8 == 0`.
- **Dyadic equiprobable law:** An equiprobable stream over $2^k$ distinct byte symbols has exact entropy $H = k$ bits/byte (e.g., 256 distinct bytes once each yields $H = 8.0$ and `entropy_q8 == 255`).

## Function Signature

```cpp
double compute_shannon_entropy(std::string_view text);
uint8_t quantize_entropy_q8(double entropy_bits);
uint8_t compute_entropy_q8(std::string_view text);
```

## Complexity Target

- **Time:** $O(\min(N, 1024) + 256)$.
- **Space:** $O(256)$ stack histogram (`std::array<uint32_t, 256>`).

## Source Reference

- [`manifold.cpp` (lines 843, 995–1035)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L995-L1035)
