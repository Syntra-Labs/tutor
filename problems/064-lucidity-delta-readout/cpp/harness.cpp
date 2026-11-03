#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 064 Lucidity & Surprise-Gated Delta Test Suite ===\n";

    // Test 1: Saturated high resonance yields q_error = 0.0 and lucidity = 1.0
    {
        LucidityEval ev = compute_lucidity_eval(0.65, 0.60);
        assert(std::abs(ev.q_error - 0.0) < 1e-9);
        assert(std::abs(ev.lucidity - 1.0) < 1e-9);
        std::cout << "  • Test 1: Saturated BMU & archetype resonance -> lucidity 1.0 ... [PASS]\n";
    }

    // Test 2: Baseline random resonance (0.50, 0.50) yields q_error = 1.5
    {
        LucidityEval ev = compute_lucidity_eval(0.50, 0.50);
        assert(std::abs(ev.q_error - 1.5) < 1e-9);
        double expected = std::clamp(1.0 - 0.5 * std::tanh(1.5) - 0.25, 0.0, 1.0);
        assert(std::abs(ev.lucidity - expected) < 1e-9);
        std::cout << "  • Test 2: Baseline random resonance quantization error ... [PASS]\n";
    }

    // Test 3: Surprise-gated delta learning rule threshold (0.35) and signed weight
    {
        // High similarity (0.70) -> surprise = 0.30 < 0.35 -> no update
        DeltaFeedback fb_low = compute_surprise_delta(0.70, true);
        assert(fb_low.updated == false);
        assert(fb_low.delta_w == 0);

        // Medium similarity (0.50) -> surprise = 0.50 >= 0.35 -> delta_w = +2
        DeltaFeedback fb_pos = compute_surprise_delta(0.50, true);
        assert(fb_pos.updated == true);
        assert(fb_pos.delta_w == 2);

        // Negative reward at similarity 0.20 -> surprise = 0.80 -> round(3.2) = 3 -> delta_w = -3
        DeltaFeedback fb_neg = compute_surprise_delta(0.20, false);
        assert(fb_neg.updated == true);
        assert(fb_neg.delta_w == -3);
        std::cout << "  • Test 3: Surprise gate deadzone and signed delta_w ... [PASS]\n";
    }

    // Performance benchmark
    constexpr size_t ITERS = 200000;
    double sum_luc = 0.0;
    int64_t sum_dw = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        double s = 0.45 + 0.001 * static_cast<double>(i & 255);
        auto ev = compute_lucidity_eval(s, s - 0.02);
        auto df = compute_surprise_delta(s, (i & 1) != 0);
        sum_luc += ev.lucidity;
        sum_dw += df.delta_w;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (sum_luc < -1.0 || sum_dw == 0x7FFFFFFFFFFFFFFFLL) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
