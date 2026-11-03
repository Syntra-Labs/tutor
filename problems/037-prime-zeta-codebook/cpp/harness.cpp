#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 037 Riemann Zeta Critical-Line Phase Codebook Suite ===\n";

    // Test 1: Identity at n = 1 (ln(1) = 0 -> cos(0) = 1, sin(0) = 0) and worked angle at p = 2
    auto cw1 = compute_zeta_codeword(1);
    for (size_t m = 0; m < 4; ++m) {
        assert(std::abs(cw1.channels[m].re - 1.0) < 1e-12);
        assert(std::abs(cw1.channels[m].im - 0.0) < 1e-12);
    }
    auto cw2 = compute_zeta_codeword(2);
    double expected_theta0 = ZETA_GAMMAS[0] * std::log(2.0);
    assert(std::abs(cw2.channels[0].re - std::cos(expected_theta0)) < 1e-12);
    assert(std::abs(cw2.channels[0].im - std::sin(expected_theta0)) < 1e-12);
    std::cout << "  • Test 1: Identity n=1 & published p=2 critical-line phase ... [PASS]\n";

    // Test 2: Multiplicative character homomorphism z_m(p * q) == z_m(p) * z_m(q)
    auto cw3 = compute_zeta_codeword(3);
    auto cw7 = compute_zeta_codeword(7);
    auto cw21 = compute_zeta_codeword(21);
    for (size_t m = 0; m < 4; ++m) {
        ComplexPhase prod = cw3.channels[m] * cw7.channels[m];
        assert(std::abs(cw21.channels[m].re - prod.re) < 1e-12);
        assert(std::abs(cw21.channels[m].im - prod.im) < 1e-12);
    }
    std::cout << "  • Test 2: Exact multiplicative homomorphism z_m(21) == z_m(3) * z_m(7) ... [PASS]\n";

    // Test 3: Q30 S^3 unit-sphere norm conservation (sum of squares ~ 2^60)
    constexpr double Q30 = static_cast<double>(1ULL << 30);
    double norm_a = 0.0, norm_b = 0.0;
    for (size_t i = 0; i < 4; ++i) {
        double a_i = static_cast<double>(cw21.s3_tuple_a_q30[i]) / Q30;
        double b_i = static_cast<double>(cw21.s3_tuple_b_q30[i]) / Q30;
        norm_a += a_i * a_i;
        norm_b += b_i * b_i;
    }
    assert(std::abs(norm_a - 1.0) < 1e-8);
    assert(std::abs(norm_b - 1.0) < 1e-8);
    std::cout << "  • Test 3: Q30 S^3 4-tuple unit norm conservation ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 200000;
    volatile int32_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 2; i < ITERS + 2; ++i) {
        auto cw = compute_zeta_codeword(i);
        sink ^= cw.s3_tuple_a_q30[0];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 037 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
