# 032: 256-Bit Unsigned Integer (`U256`) Shift, Add & Multiply

## Problem Statement

Bounded-precision dyadic arithmetic (`Dy<P>` with $P \le 120$) requires computing exact intermediate sums, differences, and products of 128-bit mantissas inside a 256-bit unsigned integer `U256 { __uint128_t hi, lo; }` representing $\text{hi} \cdot 2^{128} + \text{lo}$.

Implement `U256` with:
- `bits()`: bit-width of the integer ($0$ if zero, $128 - \text{clz}(\text{lo})$ if $\text{hi} = 0$, else $256 - \text{clz}(\text{hi})$).
- `shl(uint32_t k)`: left shift by $k \in [0, 255]$ (returns `ZERO` if $k \ge 256$).
- `shr_sticky(uint32_t k)`: right shift by $k$, returning `{shifted, sticky}` where `sticky` is `true` iff any bit shifted out was $1$.
- `add(U256 o)` and `sub(U256 o)` with 128-bit carry/borrow propagation.
- `mul_u128(__uint128_t a, __uint128_t b)`: exact $128 \times 128 \to 256$-bit product by splitting $a = a_1 2^{64} + a_0$ and $b = b_1 2^{64} + b_0$.

## Mathematical Law / Invariant

1. **Sticky Shift Conservation Law:** For any $x \in [0, 2^{256})$ and $k \in [0, 255]$, if `(q, sticky) = x.shr_sticky(k)`:
   - $q = \lfloor x / 2^k \rfloor$
   - $\text{sticky} \iff (x \bmod 2^k) \neq 0 \iff q \cdot 2^k \neq x$.
2. **Exact 256-Bit Product:** For any $a, b \in [0, 2^{128})$:
   $$\text{mul\_u128}(a, b) = a_1 b_1 2^{128} + (a_0 b_1 + a_1 b_0) 2^{64} + a_0 b_0$$
   with full carry propagation from the middle cross-terms into `hi`.

## Function Signature

```cpp
struct U256 {
    __uint128_t hi = 0;
    __uint128_t lo = 0;

    constexpr bool operator==(const U256& o) const = default;
    constexpr bool is_zero() const;
    constexpr uint32_t bits() const;
    constexpr U256 shl(uint32_t k) const;
    constexpr std::pair<U256, bool> shr_sticky(uint32_t k) const;
    constexpr U256 add(U256 o) const;
    constexpr U256 sub(U256 o) const;
    static constexpr U256 mul_u128(__uint128_t a, __uint128_t b);
};
```

## Complexity Target

- **Time:** $O(1)$ (4 64x64->128-bit hardware `mulx`/`mulq` instructions for `mul_u128`).
- **Space:** $O(1)$ (32 bytes per `U256`).
