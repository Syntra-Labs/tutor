#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 046 Hopf Fibration Projection Test Suite ===\n";

    // Test 1: Canonical poles: (1,0,0,0) -> (0,0,1) North Pole; (0,1,0,0) -> (0,0,-1) South Pole
    Vec3 north = hopf_project(Quat(1.0, 0.0, 0.0, 0.0));
    Vec3 south = hopf_project(Quat(0.0, 1.0, 0.0, 0.0));
    assert(std::abs(north.x) < 1e-12 && std::abs(north.y) < 1e-12 && std::abs(north.z - 1.0) < 1e-12);
    assert(std::abs(south.x) < 1e-12 && std::abs(south.y) < 1e-12 && std::abs(south.z + 1.0) < 1e-12);
    std::cout << "  • Test 1: Canonical North and South pole projections ... [PASS]\n";

    // Test 2: Unit-sphere landing |h(q)| == 1 for arbitrary unit quaternions
    Quat q = Quat(0.3, -0.7, 1.1, -0.5).normalize();
    Vec3 hq = hopf_project(q);
    assert(std::abs(hq.norm() - 1.0) < 1e-12);
    std::cout << "  • Test 2: Unit-sphere landing identity |h(q)| == 1 ... [PASS]\n";

    // Test 3: Exact U(1) right-fibre invariance h(q * e_psi) == h(q)
    for (double psi : {0.25, 1.13, -2.41, 3.14159265}) {
        Quat q_rot = q.mul(hopf_phase_rotor(psi));
        Vec3 h_rot = hopf_project(q_rot);
        assert(std::abs(h_rot.x - hq.x) < 1e-12);
        assert(std::abs(h_rot.y - hq.y) < 1e-12);
        assert(std::abs(h_rot.z - hq.z) < 1e-12);
    }
    std::cout << "  • Test 3: Exact U(1) fibre gauge invariance h(q * e_psi) == h(q) ... [PASS]\n";

    constexpr int ITERS = 200000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        Vec3 v = hopf_project(q);
        sink = sink + v.x + v.z;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
