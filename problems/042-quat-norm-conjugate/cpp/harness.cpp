#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 042 Quaternion Norm, Conjugate & Inverse Test Suite ===\n";

    // Test 1: Norm and normalization onto S^3
    Quat q(1.0, 2.0, -2.0, 4.0);
    assert(std::abs(q.norm_sq() - 25.0) < 1e-12);
    assert(std::abs(q.norm() - 5.0) < 1e-12);
    Quat u = q.normalize();
    assert(std::abs(u.norm() - 1.0) < 1e-12);
    assert(std::abs(u.w - 0.2) < 1e-12 && std::abs(u.x - 0.4) < 1e-12 &&
           std::abs(u.y + 0.4) < 1e-12 && std::abs(u.z - 0.8) < 1e-12);
    std::cout << "  • Test 1: S3 projection and norm ... [PASS]\n";

    // Test 2: Zero / near-zero degenerate fallback to identity (1,0,0,0)
    Quat tiny(1e-15, -1e-15, 0.0, 0.0);
    Quat id = tiny.normalize();
    assert(id.w == 1.0 && id.x == 0.0 && id.y == 0.0 && id.z == 0.0);
    std::cout << "  • Test 2: Degenerate norm (< 1e-12) fallback to identity ... [PASS]\n";

    // Test 3: Conjugate and inverse coincidence on S^3, and scaling for general quaternions
    Quat uc = u.conj();
    Quat ui = u.inverse();
    assert(std::abs(uc.w - ui.w) < 1e-12 && std::abs(uc.x - ui.x) < 1e-12 &&
           std::abs(uc.y - ui.y) < 1e-12 && std::abs(uc.z - ui.z) < 1e-12);
    Quat qi = q.inverse();
    assert(std::abs(qi.w - (1.0 / 25.0)) < 1e-12);
    assert(std::abs(qi.z - (-4.0 / 25.0)) < 1e-12);
    std::cout << "  • Test 3: Conjugate and inverse invariants ... [PASS]\n";

    constexpr int ITERS = 200000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        Quat v(1.0 + i * 1e-6, 2.0, -3.0, 4.0);
        Quat vn = v.normalize().conj();
        sink = sink + vn.w + vn.x;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
