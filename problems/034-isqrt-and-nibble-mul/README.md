# 034: Exact Integer Square Root (`isqrt`) & Base-16 LUT Multiply

## Problem Statement

In multiplier-free integer table serving (`UORLUT01` / D11), floating-point instructions (`sqrtsd`, `fmul`) are forbidden:

1. **Exact 64-Bit Integer Square Root (`isqrt`):**
   Given `uint64_t n`, compute $r = \lfloor\sqrt{n}\rfloor$ using the digit-by-digit (abacus) bitwise algorithm with **zero floating-point instructions** and **zero hardware multiplications** (safe for all $n \in [0, 2^{64}-1]$, whereas `(double)n` loses precision above $2^{53}$).
2. **Base-16 Multiples-Table Product (`lut_mul_u64`):**
   Given `uint32_t a, uint32_t b`, build a 16-entry table `T[0..15]` of multiples of `a` by repeated addition (`T[0] = 0`, `T[q] = T[q - 1] + a`), then read `T` at each of the 8 base-16 nibbles of `b` and accumulate with shifts:
   $$a \cdot b = \sum_{k=0}^{7} T\!\left[(b \gg 4k) \mathbin{\&} 15\right] \ll 4k$$

## Mathematical Law / Invariant

- **Exact Floor Square Root Inequality:** For every $n \in [0, 2^{64}-1]$, $r = \text{isqrt}(n) \in [0, 2^{32}-1]$ satisfies:
  $$r^2 \le n < (r + 1)^2$$
- **Radix-16 Decomposition Identity:** Since $b = \sum_{k=0}^{7} d_k 16^k$ with $d_k = (b \gg 4k) \mathbin{\&} 15 \in [0, 15]$ and $T[d_k] = d_k \cdot a$:
  $$\sum_{k=0}^{7} (T[d_k] \ll 4k) = \sum_{k=0}^{7} (d_k \cdot a) 2^{4k} = a \cdot b$$

## Function Signature

```cpp
constexpr uint64_t isqrt_u64(uint64_t n);
constexpr uint64_t lut_mul_u64(uint32_t a, uint32_t b);
```

## Complexity Target

- **Time:** $O(32)$ bitwise shift/sub iterations for `isqrt_u64`; 15 additions + 8 table lookups for `lut_mul_u64`.
- **Space:** $O(1)$ (16-entry `uint64_t` stack table).

## Source Reference

- [`lut-serving.md`](file:///home/yassine/code/uor-r4/docs/mechanisms/lut-serving.md#L44)
