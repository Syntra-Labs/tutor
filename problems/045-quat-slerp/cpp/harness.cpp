#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 045 Quaternion SLERP Test Suite ===\n";

    Quat q1 = Quat(1.0, 0.0, 0.0, 0.0);
    Quat q2 = Quat(0.0, 1.0, 0.0, 0.0);

    // Test 1: Endpoints and midpoint on 90-degree S^3 arc
    Quat s0 = slerp(q1, q2, 0.0);
    Quat s1 = slerp(q1, q2, 1.0);
    Quat sm = slerp(q1, q2, 0.5);
    assert(s0.geodesic_dist(q1) < 1e-12);
    assert(s1.geodesic_dist(q2) < 1e-12);
    assert(std::abs(sm.w - std::sqrt(0.5)) < 1e-12 && std::abs(sm.x - std::sqrt(0.5)) < 1e-12);
    std::cout << "  • Test 1: Endpoints and midpoint on 90-degree arc ... [PASS]\n";

    // Test 2: Constant angular speed along geodesic and shortest-path antipodal flip
    Quat p1 = Quat(1.0, 2.0, -1.0, 0.5).normalize();
    Quat p2 = Quat(-0.8, -1.5, 0.9, -0.4).normalize(); // negative dot with p1
    double total_dist = p1.geodesic_dist(p2);
    for (double t : {0.2, 0.4, 0.75}) {
        Quat pt = slerp(p1, p2, t);
        assert(std::abs(pt.norm() - 1.0) < 1e-12);
        assert(std::abs(p1.geodesic_dist(pt) - t * total_dist) < 1e-11);
    }
    std::cout << "  • Test 2: Constant angular velocity and antipodal flip ... [PASS]\n";

    // Test 3: Small-angle fallback stability (|q1 . q2| > 0.9995)
    Quat near_q1 = Quat(1.0, 1e-5, -1e-5, 0.0).normalize();
    Quat mid_near = slerp(q1, near_q1, 0.5);
    assert(std::isfinite(mid_near.w) && std::abs(mid_near.norm() - 1.0) < 1e-12);
    std::cout << "  • Test 3: Small-angle NLERP fallback stability ... [PASS]\n";

    constexpr int ITERS = 100000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        double t = (i % 100) * 0.01;
        Quat r = slerp(p1, p2, t);
        sink = sink + r.w;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
