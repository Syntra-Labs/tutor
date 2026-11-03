#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 044 Quaternion Geodesic Distance Test Suite ===\n";

    constexpr double PI = 3.14159265358979323846;

    // Test 1: Antipodal identification d(q, -q) == 0
    Quat p = Quat(1.0, 2.0, -1.0, 3.0).normalize();
    Quat neg_p(-p.w, -p.x, -p.y, -p.z);
    assert(std::abs(p.geodesic_dist(p)) < 1e-12);
    assert(std::abs(p.geodesic_dist(neg_p)) < 1e-12);
    std::cout << "  • Test 1: Antipodal identification d(q, -q) == 0 ... [PASS]\n";

    // Test 2: Orthogonal quaternions have geodesic distance pi
    Quat id = Quat::identity();
    Quat qi(0.0, 1.0, 0.0, 0.0);
    assert(std::abs(id.geodesic_dist(qi) - PI) < 1e-12);
    std::cout << "  • Test 2: Orthogonal rotors distance == pi ... [PASS]\n";

    // Test 3: Left and right multiplication bi-invariance on S^3
    Quat q = Quat(-2.0, 0.5, 4.0, 1.0).normalize();
    Quat r = Quat(0.7, -1.3, 2.1, -0.9).normalize();
    double d_base = p.geodesic_dist(q);
    double d_left = r.mul(p).geodesic_dist(r.mul(q));
    double d_right = p.mul(r).geodesic_dist(q.mul(r));
    assert(std::abs(d_base - d_left) < 1e-12);
    assert(std::abs(d_base - d_right) < 1e-12);
    std::cout << "  • Test 3: Left and right bi-invariance on S^3 ... [PASS]\n";

    constexpr int ITERS = 200000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        sink = sink + p.geodesic_dist(q);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
