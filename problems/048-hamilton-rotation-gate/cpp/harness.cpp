#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 048 Hamilton Rotation Gate Test Suite ===\n";

    Quat g(1.2, -0.8, 0.4, -1.5);
    Quat u(2.0, -1.0, 3.0, 0.5);
    Quat d_out(-0.5, 1.3, -0.7, 0.9);

    // Test 1: Norm preservation |out| == |q| * |u|
    Quat out = hamilton_gate_forward(g, u);
    double r = std::sqrt(g.norm_sq() + 1e-12);
    double expected_norm = (g.norm() / r) * u.norm();
    assert(std::abs(out.norm() - expected_norm) < 1e-12);
    std::cout << "  • Test 1: Forward norm preservation |out| == |q||u| ... [PASS]\n";

    // Test 2: Zero gate suppression inside 1e-12 guard core
    Quat zero_out = hamilton_gate_forward(Quat(0.0, 0.0, 0.0, 0.0), u);
    assert(zero_out.norm() < 1e-12);
    std::cout << "  • Test 2: Zero gate block gives zero output ... [PASS]\n";

    // Test 3: Exact backward adjoint verified against central finite differences
    HamiltonGateGrads grads = hamilton_gate_backward(g, u, d_out);
    constexpr double eps = 1e-6;
    auto loss_at = [&](const Quat& g_val, const Quat& u_val) {
        return hamilton_gate_forward(g_val, u_val).dot(d_out);
    };

    double fd_gw = (loss_at(Quat(g.w + eps, g.x, g.y, g.z), u) - loss_at(Quat(g.w - eps, g.x, g.y, g.z), u)) / (2.0 * eps);
    double fd_gx = (loss_at(Quat(g.w, g.x + eps, g.y, g.z), u) - loss_at(Quat(g.w, g.x - eps, g.y, g.z), u)) / (2.0 * eps);
    double fd_uw = (loss_at(g, Quat(u.w + eps, u.x, u.y, u.z)) - loss_at(g, Quat(u.w - eps, u.x, u.y, u.z))) / (2.0 * eps);
    double fd_uz = (loss_at(g, Quat(u.w, u.x, u.y, u.z + eps)) - loss_at(g, Quat(u.w, u.x, u.y, u.z - eps))) / (2.0 * eps);

    assert(std::abs(grads.d_g.w - fd_gw) < 1e-8);
    assert(std::abs(grads.d_g.x - fd_gx) < 1e-8);
    assert(std::abs(grads.d_u.w - fd_uw) < 1e-8);
    assert(std::abs(grads.d_u.z - fd_uz) < 1e-8);
    std::cout << "  • Test 3: Adjoint gradients match central finite differences ... [PASS]\n";

    constexpr int ITERS = 200000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        auto gr = hamilton_gate_backward(g, u, d_out);
        sink = sink + gr.d_g.w + gr.d_u.x;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
