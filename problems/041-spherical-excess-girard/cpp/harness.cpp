#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 041 Spherical Excess (Girard's Theorem) Test Suite ===\n";

    constexpr double PI = 3.14159265358979323846;
    Vec3 vx{1.0, 0.0, 0.0};
    Vec3 vy{0.0, 1.0, 0.0};
    Vec3 vz{0.0, 0.0, 1.0};

    // Test 1: Octant triangle has three 90-degree (pi/2) angles and area pi/2 (1/8 of 4*pi)
    assert(std::abs(spherical_triangle_angle(vx, vy, vz) - (PI * 0.5)) < 1e-12);
    assert(std::abs(spherical_excess_triangle(vx, vy, vz) - (PI * 0.5)) < 1e-12);
    std::cout << "  • Test 1: Octant triangle area == pi/2 ... [PASS]\n";

    // Test 2: Half-octant triangle (bisecting xy arc at 45 degrees) has area pi/4
    Vec3 vxy{std::sqrt(0.5), std::sqrt(0.5), 0.0};
    double e1 = spherical_excess_triangle(vx, vxy, vz);
    double e2 = spherical_excess_triangle(vxy, vy, vz);
    assert(std::abs(e1 - (PI * 0.25)) < 1e-12);
    assert(std::abs(e1 + e2 - (PI * 0.5)) < 1e-12);
    std::cout << "  • Test 2: Additivity under geodesic bisection (pi/4 + pi/4 == pi/2) ... [PASS]\n";

    // Test 3: Spherical square (4 vertices around North Pole at 60 deg colatitude)
    double s = std::sqrt(0.5), c = std::sqrt(0.5);
    std::vector<Vec3> quad = {
        {s, 0.0, c},
        {0.0, s, c},
        {-s, 0.0, c},
        {0.0, -s, c}
    };
    double quad_area = spherical_excess_polygon(quad);
    double tri_sum = spherical_excess_triangle(quad[0], quad[1], quad[2]) +
                     spherical_excess_triangle(quad[0], quad[2], quad[3]);
    assert(std::abs(quad_area - tri_sum) < 1e-12);
    assert(quad_area > 0.0);
    std::cout << "  • Test 3: Geodesic quadrilateral triangulation consistency ... [PASS]\n";

    constexpr int ITERS = 100000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        sink = sink + spherical_excess_triangle(vx, vxy, vz);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
