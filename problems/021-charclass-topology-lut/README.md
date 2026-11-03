# 021: Constexpr Character-Class Topology & Transition Flux

## Problem Statement

Before parsing or tokenizing raw byte streams, structural boundaries can be sensed directly from the local character-class topology. Every byte $b \in [0, 255]$ is mapped at compile time via a 256-entry lookup table (`CharClassLUT`) into one of 7 disjoint topological character classes:

- `SPACE (0)`: `' '`, `'\t'`, `'\r'`, `'\n'`
- `LOWER (1)`: `'a'`..`'z'`
- `UPPER (2)`: `'A'`..`'Z'`
- `DIGIT (3)`: `'0'`..`'9'`
- `OP_MATH (4)`: `+-*/=<>!&|^~%\`
- `DELIM (5)`: `(){}[].,;:_#@$"'` and `` ` ``
- `OTHER (6)`: all remaining byte values

Given a byte span, we measure its **transition flux** (mean character-class transition rate) and operator/delimiter densities. For strings longer than 1024 bytes, the stream is strided with `step = text.size() / 1024` (`step = 1` when `text.size() <= 1024`).

## Mathematical Law / Invariant

Let $c_0, c_1, \dots, c_{M-1}$ be the $M$ sampled bytes at indices $i = 0, \text{step}, 2\cdot\text{step}, \dots < N$, and let $\mathcal{C}(c_k) \in \{0, \dots, 6\}$ denote the character class of $c_k$:

$$\text{Transition Flux} = \frac{1}{M} \sum_{k=1}^{M-1} \mathbb{I}\!\left(\mathcal{C}(c_k) \neq \mathcal{C}(c_{k-1})\right) \in [0, 1)$$

- **Constant-class invariant:** If all sampled bytes belong to the same `CharClass`, transition flux is exactly $0.0$.
- **Alternating-class bound:** For $M \ge 1$ alternating classes, transition flux is $\frac{M-1}{M}$.

## Function Signature

```cpp
enum class CharClass : uint8_t {
    SPACE = 0, LOWER = 1, UPPER = 2, DIGIT = 3, OP_MATH = 4, DELIM = 5, OTHER = 6
};

struct CharClassLUT {
    std::array<CharClass, 256> table{};
    constexpr CharClassLUT();
};

struct TopologyFluxStats {
    double transition_flux = 0.0;
    double op_density      = 0.0;
    double delim_density   = 0.0;
};

constexpr CharClass classify_byte(uint8_t c);
TopologyFluxStats compute_topology_flux(std::string_view text);
```

## Complexity Target

- **Time:** $O(\min(N, 1024))$ table lookups per span (`constexpr` $O(1)$ per byte).
- **Space:** $O(1)$ (256-byte static constexpr LUT).

## Source Reference

- [`manifold.cpp` (lines 963–1035)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L963-L1035)
