#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 039 Spherical Haversine Test Suite ===\n";

    // Test 1: Identical points and quarter-equator (90 deg = pi/2 rad)
    assert(std::abs(haversine_distance_rad(0.0, 0.0, 0.0, 0.0)) < 1e-12);
    assert(std::abs(haversine_distance_deg(0.0, 0.0, 0.0, 90.0) - 90.0) < 1e-9);
    assert(std::abs(haversine_distance_rad(0.0, 0.0, 90.0, 0.0) - (SPHERE_PI * 0.5)) < 1e-12);
    std::cout << "  • Test 1: Zero distance and 90-degree quadrant arcs ... [PASS]\n";

    // Test 2: Antipodal points (clamped at a=1.0 -> pi radians = 180 degrees)
    assert(std::abs(haversine_distance_deg(37.7749, -122.4194, -37.7749, 57.5806) - 180.0) < 1e-9);
    assert(std::abs(haversine_distance_rad(90.0, 0.0, -90.0, 0.0) - SPHERE_PI) < 1e-12);
    std::cout << "  • Test 2: Antipodal stability (180 degrees) ... [PASS]\n";

    // Test 3: Symmetry and Spherical Triangle Inequality
    double d_ab = haversine_distance_rad(51.5074, -0.1278, 40.7128, -74.0060);
    double d_ba = haversine_distance_rad(40.7128, -74.0060, 51.5074, -0.1278);
    double d_bc = haversine_distance_rad(40.7128, -74.0060, 35.6762, 139.6503);
    double d_ac = haversine_distance_rad(51.5074, -0.1278, 35.6762, 139.6503);
    assert(std::abs(d_ab - d_ba) < 1e-12);
    assert(d_ac <= d_ab + d_bc + 1e-12);
    std::cout << "  • Test 3: Symmetry and spherical triangle inequality ... [PASS]\n";

    // Benchmark loop
    constexpr int ITERS = 100000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        double lat = (i % 180) - 90.0;
        double lon = (i % 360) - 180.0;
        sink = sink + haversine_distance_rad(lat, lon, -lat * 0.5, lon * 0.5);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
