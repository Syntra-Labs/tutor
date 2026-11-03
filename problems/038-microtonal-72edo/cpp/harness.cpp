#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 038 72-EDO Microtonal Frequency & Consonance Suite ===\n";

    // Test 1: Base frequency, tritone (step 36 = sqrt(2)), and octave periodicity
    assert(std::abs(freq_72edo(0) - 261.63) < 1e-9);
    assert(std::abs(freq_72edo(72) - 261.63) < 1e-9);
    assert(std::abs(freq_72edo(36) - 261.63 * std::sqrt(2.0)) < 1e-9);
    for (uint32_t c = 0; c < 72; ++c) {
        assert(std::abs(freq_72edo(c) - freq_72edo(c + 72)) < 1e-12);
    }
    std::cout << "  • Test 1: 72-EDO base frequency, tritone sqrt(2) & modulo-72 periodicity ... [PASS]\n";

    // Test 2: Duration schedule ((45 + ((5*c) % 6) * 20) / 1000.0) / speed
    assert(std::abs(note_duration_sec(0, 1.0) - 0.045) < 1e-9);
    assert(std::abs(note_duration_sec(1, 1.0) - 0.145) < 1e-9); // (5*1)%6 == 5 -> 45 + 100 = 145ms
    assert(std::abs(note_duration_sec(2, 2.0) - 0.0625) < 1e-9); // (5*2)%6 == 4 -> 45 + 80 = 125ms / 2 = 62.5ms
    std::cout << "  • Test 2: 6-step coprime duration schedule & speed scaling ... [PASS]\n";

    // Test 3: Circular pitch-class distance on Z_72
    assert(pitch_class_distance_72edo(0, 72) == 0);
    assert(pitch_class_distance_72edo(1, 71) == 2);
    assert(pitch_class_distance_72edo(0, 36) == 36);
    assert(pitch_class_distance_72edo(10, 60) == 22); // 72 - 50 = 22
    std::cout << "  • Test 3: Circular geodesic distance on Z_72 ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 500000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (uint32_t i = 0; i < ITERS; ++i) {
        sink += freq_72edo(i) + note_duration_sec(i, 1.0);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 038 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
