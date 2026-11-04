# 033: Bounded-Precision Dyadic `Dy<P>` Canonicalization & Rounding

## Problem Statement

A bounded-precision dyadic number `Dy<P>` ($8 \le P \le 120$) represents $m \cdot 2^e$ with a signed 128-bit mantissa `m` of at most $P$ significant bits ($|m| < 2^P$) and a signed 32-bit exponent `e`.

Every `Dy<P>` value is stored **canonically**:
- Zero is stored uniquely as `m = 0, e = 0`.
- Every non-zero value has an **odd** mantissa (`(m & 1) != 0`), obtained by shifting out all trailing zeros of $|m|$ and adding that count to `e`.

Implement `Dy<P>::round(bool neg, U256 mag, int64_t exp, bool sticky)` which rounds $(-1)^{\text{neg}} (\text{mag} + f) 2^{\text{exp}}$ (where $0 < f < 1 \iff \text{sticky}$) to at most $P$ significant bits using **round-to-nearest, ties-to-even**, and implement `Dy<P>::add` and `Dy<P>::mul`.

## Mathematical Law / Invariant

Given `b = mag.bits()`:
1. If `mag.is_zero()`, return `{0, 0}`.
2. If `sticky && b <= P + 1`, left-shift `mag` by `k = P + 2 - b` and subtract `k` from `exp` so the sticky fraction lies strictly below the round bit.
3. Recompute `b = mag.bits()`. If `b > P`:
   - Let `k = b - P`, and `(kept, low) = mag.shr_sticky(k - 1)`.
   - `round_bit = (kept.lo & 1) != 0` and `m = kept.lo >> 1`.
   - **Ties-to-even rule:** Increment `m += 1` iff `round_bit && (low || sticky || (m & 1) != 0)`.
   - `e = exp + k`. If carry overflowed $P$ bits (`(m >> P) != 0`), shift `m >>= 1` and `e += 1`.
4. Canonicalize `m` by shifting right by `tz = ctz(m)` and incrementing `e += tz`.

## Function Signature

```cpp
template <uint32_t P = 120>
struct Dy {
    __int128 m = 0;
    int32_t e = 0;

    static constexpr Dy round(bool neg, U256 mag, int64_t exp, bool sticky);
    static constexpr Dy from_i64(int64_t v);
    constexpr Dy add(Dy o) const;
    constexpr Dy mul(Dy o) const;
};
```

## Complexity Target

- **Time:** $O(1)$ per `round`, `add`, and `mul` (pure integer arithmetic, zero floats).
- **Space:** $O(1)$.
