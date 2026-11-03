#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 047 Hopf Kept-Fibre & 512-Sector Quantizer Test Suite ===\n";

    // Test 1: Exact round-trip reconstruction q -> (h(q), psi(q)) -> q
    Quat q = Quat(0.6, -0.3, 0.5, -0.5291502622129181).normalize();
    Vec3 base = hopf_project(q);
    double psi = hopf_fibre_angle(q);
    Quat q_rec = reconstruct_from_hopf(base, psi);
    assert(std::abs(q_rec.w - q.w) < 1e-11 && std::abs(q_rec.x - q.x) < 1e-11 &&
           std::abs(q_rec.y - q.y) < 1e-11 && std::abs(q_rec.z - q.z) < 1e-11);
    std::cout << "  • Test 1: Exact bijection q == reconstruct_from_hopf(h(q), psi(q)) ... [PASS]\n";

    // Test 2: South-pole singularity handling (w = z = 0)
    Quat q_south(0.0, 0.6, 0.8, 0.0);
    Vec3 s_south = hopf_project(q_south);
    double psi_south = hopf_fibre_angle(q_south);
    Quat q_south_rec = reconstruct_from_hopf(s_south, psi_south);
    assert(std::abs(q_south_rec.x - 0.6) < 1e-12 && std::abs(q_south_rec.y - 0.8) < 1e-12);
    std::cout << "  • Test 2: South-pole chart fallback reconstruction ... [PASS]\n";

    // Test 3: 512-sector boundary quantization
    assert(quantize_hopf_sector_512(Vec3{-1.0, -1.0, -1.0}) == 0);
    assert(quantize_hopf_sector_512(Vec3{1.0, 1.0, 1.0}) == 511);
    // (0, 0, 1) -> bx=4, by=4, bz=7 -> (4<<6)|(4<<3)|7 = 256 + 32 + 7 = 295
    assert(quantize_hopf_sector_512(Vec3{0.0, 0.0, 1.0}) == 295);
    std::cout << "  • Test 3: 512-sector octree quantization boundaries ... [PASS]\n";

    constexpr int ITERS = 100000;
    volatile uint32_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        Quat qr = reconstruct_from_hopf(base, psi);
        sink = sink + quantize_hopf_sector_512(hopf_project(qr));
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
