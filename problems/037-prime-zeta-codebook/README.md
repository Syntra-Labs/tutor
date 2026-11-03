# 037: Riemann Zeta Critical-Line Phase Codebook

## Problem Statement

The non-trivial zeros of the Riemann zeta function on the critical line $\zeta(\tfrac{1}{2} + i\gamma_m) = 0$ have fixed imaginary frequencies:

$$\gamma_0 = 14.134725141734693790, \quad \gamma_1 = 21.022039638771554993, \quad \gamma_2 = 25.010857580145688763, \quad \gamma_3 = 30.424876125859513210$$

For a prime (or positive integer) $p$, each frequency $\gamma_m$ defines a complex phase angle $\theta_m(p) = \gamma_m \ln p$ and a unit complex rotor $z_m(p) = e^{i \gamma_m \ln p} = p^{i\gamma_m} = (\cos(\gamma_m \ln p), \sin(\gamma_m \ln p)) \in U(1)$.

In the `prime_zeta` constructor, the four complex channels $(z_0, z_1, z_2, z_3) \in \mathbb{C}^4$ are grouped into two 4-real tuples:
- Tuple A: $\frac{1}{\sqrt{2}}(\operatorname{Re} z_0, \operatorname{Im} z_0, \operatorname{Re} z_1, \operatorname{Im} z_1) \in S^3$
- Tuple B: $\frac{1}{\sqrt{2}}(\operatorname{Re} z_2, \operatorname{Im} z_2, \operatorname{Re} z_3, \operatorname{Im} z_3) \in S^3$
and quantized to signed 32-bit fixed-point Q30 integers (`round(coord * (1 << 30))`).

## Mathematical Law / Invariant

1. **Multiplicative-to-Unitary Character Homomorphism:**
   Because $\ln(a \cdot b) = \ln a + \ln b$, the map $p \mapsto z_m(p) = e^{i\gamma_m \ln p}$ is an exact group homomorphism from $(\mathbb{R}_{>0}, \times)$ into the unit circle $(U(1), \cdot_{\mathbb{C}})$:
   $$z_m(a \cdot b) = z_m(a) \cdot_{\mathbb{C}} z_m(b) = \bigl(\operatorname{Re} z_m(a)\operatorname{Re} z_m(b) - \operatorname{Im} z_m(a)\operatorname{Im} z_m(b),\; \operatorname{Re} z_m(a)\operatorname{Im} z_m(b) + \operatorname{Im} z_m(a)\operatorname{Re} z_m(b)\bigr)$$
2. **Unit $S^3$ Norm Invariant:**
   Since $|z_m(p)|^2 = \cos^2\theta_m + \sin^2\theta_m = 1$, each normalized 4-real tuple $(\operatorname{Re} z_{2r}, \operatorname{Im} z_{2r}, \operatorname{Re} z_{2r+1}, \operatorname{Im} z_{2r+1}) / \sqrt{2}$ lies strictly on the unit 3-sphere $S^3$ with Euclidean norm $1.0$.

## Function Signature

```cpp
struct ComplexPhase {
    double re = 1.0;
    double im = 0.0;
    constexpr ComplexPhase operator*(const ComplexPhase& o) const;
};

struct ZetaCodeword {
    std::array<ComplexPhase, 4> channels{};
    std::array<int32_t, 4> s3_tuple_a_q30{};
    std::array<int32_t, 4> s3_tuple_b_q30{};
};

ZetaCodeword compute_zeta_codeword(uint64_t n);
```

## Complexity Target

- **Time:** $O(1)$ (`1` natural logarithm and `4` `sin`/`cos` evaluations per integer $n$).
- **Space:** $O(1)$.

## Source Reference

- [`primes-and-zeta.md` (lines 9–25)](file:///home/yassine/code/uor-r4/docs/geometry/primes-and-zeta.md#L9-L25)
