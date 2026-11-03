#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

#include "solution.hpp"

static double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

int main() {
    std::cout << "=== Running 040 Spherical Forward Waypoint Test Suite ===\n";

    // Test 1: Forward 90 deg from North Pole (0,0,1) along (1,0,0) lands at (1,0,0) with heading (0,0,-1)
    SphericalTurtle t1 = run_spherical_path({{Op::Forward, 90.0}});
    assert(std::abs(t1.pos.x - 1.0) < 1e-12 && std::abs(t1.pos.y) < 1e-12 && std::abs(t1.pos.z) < 1e-12);
    assert(std::abs(t1.heading.x) < 1e-12 && std::abs(t1.heading.y) < 1e-12 && std::abs(t1.heading.z + 1.0) < 1e-12);
    std::cout << "  • Test 1: Quarter-meridian forward transport ... [PASS]\n";

    // Test 2: Octant spherical triangle: 3 x [Forward(90), Turn(90)] returns to start state!
    // Notice: on S^2, turning left 90 deg (exterior angle 90 deg -> interior angle 90 deg) 3 times closes the octant!
    std::vector<Command> octant = {
        {Op::Forward, 90.0}, {Op::Turn, 90.0},
        {Op::Forward, 90.0}, {Op::Turn, 90.0},
        {Op::Forward, 90.0}, {Op::Turn, 90.0}
    };
    SphericalTurtle t2 = run_spherical_path(octant);
    assert(std::abs(t2.pos.x) < 1e-12 && std::abs(t2.pos.y) < 1e-12 && std::abs(t2.pos.z - 1.0) < 1e-12);
    assert(std::abs(t2.heading.x - 1.0) < 1e-12 && std::abs(t2.heading.y) < 1e-12 && std::abs(t2.heading.z) < 1e-12);
    std::cout << "  • Test 2: Octant triangle holonomy closure ... [PASS]\n";

    // Test 3: Orthonormality invariant under arbitrary multi-step path
    std::vector<Command> path = {
        {Op::Forward, 37.5}, {Op::Turn, -42.0},
        {Op::Forward, 112.3}, {Op::Turn, 128.7},
        {Op::Forward, 64.1}, {Op::Turn, -19.4}
    };
    SphericalTurtle t3 = run_spherical_path(path);
    assert(std::abs(dot(t3.pos, t3.pos) - 1.0) < 1e-12);
    assert(std::abs(dot(t3.heading, t3.heading) - 1.0) < 1e-12);
    assert(std::abs(dot(t3.pos, t3.heading)) < 1e-12);
    std::cout << "  • Test 3: Unit tangent bundle invariant (|p|=1, |t|=1, p.t=0) ... [PASS]\n";

    constexpr int ITERS = 50000;
    volatile double sink = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        SphericalTurtle res = run_spherical_path(octant);
        sink = sink + res.pos.z;
    }
    auto end = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(end - start).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
