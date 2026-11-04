# 031: Bit-Exact IEEE-754 `binary32` to Dyadic Rational Decoder

## Problem Statement

Every finite IEEE-754 `binary32` (`float`) value is an exact dyadic rational $m \cdot 2^e$. To ingest floating-point problem weights into an exact integer solver without rounding, decode raw 32-bit IEEE-754 bit patterns `uint32_t bits` into their canonical reduced dyadic form `ExactDyadicF32 { int64_t m; int32_t e; }`:

1. Extract `negative = (bits >> 31) != 0`, `exp_field = (bits >> 23) & 0xff`, and `frac = bits & 0x7fffff`.
2. **Non-Finite Rejection:** If `exp_field == 0xff` ($\pm\infty$ or `NaN`), return `std::nullopt`.
3. **Raw Unreduced Decode:**
   - If `exp_field == 0` (zero or subnormal): `raw_m = frac`, `raw_e = -149`.
   - If `1 <= exp_field <= 254` (normal): `raw_m = frac | (1u << 23)`, `raw_e = exp_field - 150`.
4. **Canonical Reduction:**
   - If `raw_m == 0` (including `-0.0f`, `0x80000000u`), return `{0, 0}`.
   - Otherwise, factor out all trailing powers of 2 from `raw_m` (`tz = std::countr_zero(raw_m)`):
     $$m = (-1)^{\text{negative}} \cdot (\text{raw\_m} \gg \text{tz}), \qquad e = \text{raw\_e} + \text{tz}$$
     so that $m$ is always odd and $|m| < 2^{24}$.

## Mathematical Law / Invariant

- **Unique Canonical Representation:** Every finite `binary32` value $x$ has a unique pair $(m, e)$ where either $(m, e) = (0, 0)$ or $m$ is odd and $x = m \cdot 2^e$ exactly.
- **Signed Zero Collapse:** Both `+0.0f` (`0x00000000`) and `-0.0f` (`0x80000000`) map to `{0, 0}`.

## Function Signature

```cpp
struct ExactDyadicF32 {
    int64_t m = 0; // odd mantissa (or 0 when value is 0)
    int32_t e = 0; // base-2 exponent (0 when value is 0)
    constexpr bool operator==(const ExactDyadicF32& o) const = default;
};

constexpr std::optional<ExactDyadicF32> decode_f32_bits(uint32_t bits);
```

## Complexity Target

- **Time:** $O(1)$ bitwise operations (zero floating-point instructions).
- **Space:** $O(1)$.
