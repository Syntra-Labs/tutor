#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 043 Quaternion Hamilton Product & Rotor Test Suite ===\n";

    constexpr double PI = 3.14159265358979323846;

    // Test 1: Fundamental Hamilton relations i*j = k, j*i = -k, i^2 = j^2 = k^2 = -1
    Quat qi(0.0, 1.0, 0.0, 0.0);
    Quat qj(0.0, 0.0, 1.0, 0.0);
    Quat qk(0.0, 0.0, 0.0, 1.0);
    Quat ij = qi.mul(qj);
    Quat ji = qj.mul(qi);
    assert(std::abs(ij.z - 1.0) < 1e-12 && std::abs(ji.z + 1.0) < 1e-12);
    assert(std::abs(qi.mul(qi).w + 1.0) < 1e-12);
    std::cout << "  • Test 1: Non-commutativity (i*j = k, j*i = -k) ... [PASS]\n";

    // Test 2: Norm multiplicativity |p * q| == |p| * |q| and anti-automorphism conj(p*q) == conj(q)*conj(p)
    Quat p(1.5, -2.0, 0.5, 3.0);
    Quat q(-0.5, 1.0, 2.5, -1.5);
    Quat pq = p.mul(q);
    assert(std::abs(pq.norm() - p.norm() * q.norm()) < 1e-12);
    Quat cpq = pq.conj();
    Quat cqc_p = q.conj().mul(p.conj());
    assert(std::abs(cpq.w - cqc_p.w) < 1e-12 && std::abs(cpq.x - cqc_p.x) < 1e-12 &&
           std::abs(cpq.y - cqc_p.y) < 1e-12 && std::abs(cpq.z - cqc_p.z) < 1e-12);
    std::cout << "  • Test 2: Norm multiplicativity and conjugate anti-automorphism ... [PASS]\n";

    // Test 3: Axis-angle 90-degree rotor around Z rotates X=(0,1,0,0) to Y=(0,0,1,0)
    Quat rz90 = make_rotor(PI * 0.5, 0.0, 0.0, 5.0);
    assert(std::abs(rz90.norm() - 1.0) < 1e-12);
    Quat rotated = qi.rotate(rz90);
    assert(std::abs(rotated.w) < 1e-12 && std::abs(rotated.x) < 1e-12 &&
           std::abs(rotated.y - 1.0) < 1e-12 && std::abs(rotated.z) < 1e-12);
    std::cout << "  • Test 3: 90-deg Z-rotor sandwich action rotates i -> j ... [PASS]\n";

    constexpr int ITERS = 200000;
    volatile double sink = 0.0;
    Quat state = Quat::identity();
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        state = state.rotate(rz90);
        sink = sink + state.x;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
