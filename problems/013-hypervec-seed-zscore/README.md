# 013: Deterministic Hypervector Expansion & Z-Score Metric

## Problem Statement

In 8192-bit Vector Symbolic Architectures (VSA), atomic symbol vectors are expanded deterministically on-the-fly from a 64-bit seed using `splitmix64`, and pairwise similarity is measured in standard deviations ($z$-score) from random chance.

Implement on `Hypervec8192`:
1. `static Hypervec8192 from_seed(uint64_t seed)`:
   - Initialize `uint64_t state = seed;` and populate `words[0..127]` with consecutive calls to `splitmix64(state)`.
2. `uint32_t hamming(const Hypervec8192& other) const`:
   - Sum `std::popcount(words[i] ^ other.words[i])` across all 128 words.
3. `double z_score(const Hypervec8192& other) const`:
   - Compute $z = \frac{4096.0 - d_H(A, B)}{\frac{1}{2}\sqrt{8192.0}}$ where $\sigma = \frac{1}{2}\sqrt{8192} = 32\sqrt{2} \approx 45.254834$.

## Mathematical Law / Invariant

- **Concentration of Measure on $\{0, 1\}^{8192}$:** Two independent random hypervectors have Hamming distance $d_H \sim \mathcal{B}(8192, 0.5)$ with mean $\mu = \frac{D}{2} = 4096$ and standard deviation $\sigma = \frac{\sqrt{D}}{2} = 32\sqrt{2} \approx 45.254834$.
- **Quasi-Orthogonality:** Distinct seeds yield $|z| < 3.5$ with overwhelming probability ($1 - 4.6 \times 10^{-4}$), whereas identical vectors achieve $z = +64\sqrt{2} \approx +90.509668$ and antipodal vectors achieve $z = -64\sqrt{2} \approx -90.509668$.

## Function Signature

```cpp
uint64_t splitmix64(uint64_t& state);

struct Hypervec8192 {
    static constexpr size_t HV_BITS = 8192;
    static constexpr size_t HV_WORDS = 128;
    std::array<uint64_t, HV_WORDS> words{};

    static Hypervec8192 from_seed(uint64_t seed);
    uint32_t hamming(const Hypervec8192& other) const;
    double z_score(const Hypervec8192& other) const;
};
```

## Complexity Target

- **Time:** $O(128)$ words (~120 cycles for expansion or $z$-score).
- **Space:** 1024 bytes per `Hypervec8192`.
