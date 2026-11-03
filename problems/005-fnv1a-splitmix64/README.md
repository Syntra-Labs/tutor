# 005: FNV-1a Hash & SplitMix64 Bijective Finalizer

## Problem Statement

Deterministic hyperdimensional substrates require mapping arbitrary byte spans into 64-bit seeds (`fnv1a_64`) and expanding those seeds into uncorrelated 64-bit words via a bijective mixer (`splitmix64`).

Implement:
1. `uint64_t fnv1a_64(const void* data, size_t len, uint64_t basis = 14695981039346656037ULL)`
   - For each byte $b_i \in \text{data}$: XOR into the hash accumulator, then multiply by the 64-bit FNV prime `1099511628211ULL` (`0x100000001B3ULL`).
2. `uint64_t splitmix64(uint64_t& state)`
   - Advance `state += 0x9E3779B97F4A7C15ULL`, then apply the Stafford Mix13 bijective finalizer:
     - `z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL`
     - `z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL`
     - `return z ^ (z >> 31)`

## Mathematical Law / Invariant

- **Bijectivity on $\mathbb{Z}_{2^{64}}$:** Every step of `splitmix64`'s output mixing function is invertible: $z \mapsto z \oplus (z \gg k)$ is a triangular GF(2) automorphism, and multiplication by an odd constant is an automorphism of the ring $(\mathbb{Z}/2^{64}\mathbb{Z}, +)$ since $\gcd(\text{odd}, 2^{64}) = 1$. Therefore, $2^{64}$ distinct states yield $2^{64}$ distinct outputs with zero collisions.
- **Avalanche Criterion:** Flipping a single bit of the input seed flips approximately $32$ of the $64$ output bits on average.

## Function Signature

```cpp
uint64_t fnv1a_64(const void* data, size_t len, uint64_t basis = 14695981039346656037ULL);
uint64_t fnv1a_str(std::string_view s, uint64_t basis = 14695981039346656037ULL);
uint64_t splitmix64(uint64_t& state);
```

## Complexity Target

- **Time:** $O(\text{len})$ for `fnv1a_64` (~1.5 cycles/byte); $O(1)$ (6 instructions) for `splitmix64`.
- **Space:** $O(1)$ registers only.

## Source

- [`manifold.cpp` (lines 390–409)](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L390-L409)
