#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 049 Dyadic Recurrence Gate & Secant Credit Test Suite ===\n";

    // Test 1: Dyadic rate table values and unit circle invariant lambda^2 + kappa^2 == 1
    for (int i = 0; i < 16; ++i) {
        double score = (i + 0.5) / 16.0;
        assert(rate_index_of(score) == static_cast<uint32_t>(i));
        auto [lam, kap] = rate_select_forward(score, 0.0);
        double expected_lam = std::ldexp(1.0, -(i + 1));
        assert(std::abs(lam - expected_lam) < 1e-15);
        assert(std::abs(lam * lam + kap * kap - 1.0) < 1e-15);
    }
    assert(rate_index_of(1.0) == 15);
    std::cout << "  • Test 1: 16-entry dyadic rate and keep table invariants ... [PASS]\n";

    // Test 2: Boundary and interior secant surrogate credit
    // At i=0 (s=0.01): only right neighbor i=1 exists (lambda_0 = 0.5, lambda_1 = 0.25 -> diff = -0.25)
    double grad0 = rate_select_secant_backward(0.01, 0.0, 1.0, 0.0);
    assert(std::abs(grad0 - (-0.25 * 16.0)) < 1e-12);
    // At i=1 (s=0.10): left i=0 (0.5) and right i=2 (0.125), average secant slope = ((0.125 - 0.25) - (0.5 - 0.25))/2 * 16 = -0.1875 * 16 = -3.0
    double grad1 = rate_select_secant_backward(0.10, 0.0, 1.0, 0.0);
    assert(std::abs(grad1 - (-3.0)) < 1e-12);
    std::cout << "  • Test 2: Boundary and interior secant surrogate slopes ... [PASS]\n";

    // Test 3: Quaternion recurrence step forward
    Quat h0(1.0, 0.0, 0.0, 0.0);
    Quat u(0.0, 1.0, 0.0, 0.0); // i
    Quat b(0.0, 0.0, 1.0, 0.0); // j
    Quat h1 = recurrence_step_forward(h0, u, b, 0.01, 0.0); // i=0: lambda=0.5, kappa=sqrt(3)/2
    assert(std::abs(h1.x - 0.5) < 1e-12);
    assert(std::abs(h1.y - std::sqrt(0.75)) < 1e-12);
    std::cout << "  • Test 3: Quaternion recurrence carry + keep injection ... [PASS]\n";

    constexpr int ITERS = 200000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        double g = (i & 15) * (1.0 / 16.0);
        sink = sink + rate_select_secant_backward(g, 0.02, 1.0, -0.5);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
