# 035: 4-Bit Group-Scaled LUT Matrix-Vector Projection

## Problem Statement

In `UORLUT01` (`D11` integer table serving), a matrix-vector projection $y = W x$ over input dimension $D$ (a multiple of the group size $G = 32$) stores weights as 4-bit signed codes $q_{i,j} \in [-8, 7]$ (encoded as 4-bit unsigned index $u_{i,j} = q_{i,j} + 8 \in [0, 15]$, packed 2 weights per byte: low nibble `byte & 0x0F` is even column $2k$, high nibble `(byte >> 4) & 0x0F` is odd column $2k+1$).

Each group $g \in [0, D/32)$ of 32 consecutive weights in row $i$ shares a 1-byte dyadic scale `scale_byte`:
- Low nibble `m = scale_byte & 0x0F` $\in [0, 15]$ is the 4-bit unsigned mantissa.
- High nibble `e = (scale_byte >> 4) & 0x0F` $\in [0, 15]$ is the 4-bit shift exponent.
- Group scale value is $s_{i,g} = m \cdot 2^e = m \ll e$.

Instead of multiplying $q_{i,j} \cdot x_j$ for every row $i$, the engine precomputes once per activation $x_j$ a 16-entry signed lookup table $T_j[u] = (u - 8) \cdot x_j$ for $u \in [0, 15]$ using **additions only**, and then evaluates every row $i$ by table lookups, group-scale shifts/adds, and accumulation.

## Mathematical Law / Invariant

1. **Multiplier-Free Activation Table Construction:**
   For each column $j \in [0, D)$:
   $$T_j[8] = 0, \qquad T_j[8 + k] = T_j[8 + k - 1] + x_j, \qquad T_j[8 - k] = T_j[8 - k + 1] - x_j$$
   so that $T_j[u] = (u - 8) x_j$ for all $u \in \{0, \dots, 15\}$ with zero `*` instructions.
2. **Group-Scaled Dot Product Identity:**
   For each output row $i$ and group $g \in [0, D/32)$:
   $$\text{group\_sum}_{i,g} = \sum_{c=0}^{31} T_{32g + c}[u_{i, 32g + c}], \qquad y_i = \sum_{g=0}^{D/32 - 1} \text{scale\_mul}(\text{group\_sum}_{i,g}, m_{i,g}) \ll e_{i,g}$$
   where multiplying by the 4-bit mantissa $m \in [0, 15]$ is also just a 16-entry or 4-bit shift-add without `*`.

## Function Signature

```cpp
using ActivationLUT = std::array<int64_t, 16>;

void build_activation_luts(const int32_t* x, size_t d_in, ActivationLUT* luts_out);

void uorlut01_project(
    const uint8_t* packed_weights, // [d_out][d_in / 2] (low nibble = col 2k, high nibble = col 2k+1)
    const uint8_t* group_scales,   // [d_out][d_in / 32] (low nibble = m, high nibble = e)
    const int32_t* x,              // [d_in]
    size_t d_out,
    size_t d_in,                   // multiple of 32
    int64_t* y_out                 // [d_out]
);
```

## Complexity Target

- **Time:** $15 \cdot D_{\text{in}}$ additions to build `luts_out`, plus $D_{\text{out}} \cdot D_{\text{in}}$ table reads and additions (zero `*` instructions).
- **Space:** $O(D_{\text{in}})$ for the precomputed 16-entry activation tables.
