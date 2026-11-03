#pragma once

#include <array>
#include <cmath>
#include <cstdint>

inline constexpr std::array<double, 4> ZETA_GAMMAS = {
    14.134725141734693790,
    21.022039638771554993,
    25.010857580145688763,
    30.424876125859513210
};

struct ComplexPhase {
    double re = 1.0;
    double im = 0.0;

    constexpr ComplexPhase operator*(const ComplexPhase& o) const {
        return {re * o.re - im * o.im, re * o.im + im * o.re};
    }
};

struct ZetaCodeword {
    std::array<ComplexPhase, 4> channels{};
    std::array<int32_t, 4> s3_tuple_a_q30{};
    std::array<int32_t, 4> s3_tuple_b_q30{};
};

inline ZetaCodeword compute_zeta_codeword(uint64_t n) {
    // TODO:
    // 1. For each m in 0..3, theta_m = ZETA_GAMMAS[m] * std::log(static_cast<double>(n)),
    //    channels[m] = {std::cos(theta_m), std::sin(theta_m)}
    // 2. Normalize pairs (channels[0], channels[1]) and (channels[2], channels[3]) onto S^3
    //    by multiplying by 1.0 / sqrt(2.0), and quantize to Q30 via std::llround(val * (1ULL << 30))
    (void)n;
    return {};
}
