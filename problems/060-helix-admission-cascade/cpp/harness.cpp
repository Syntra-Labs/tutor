#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 060 Helix Admission Cascade Test Suite ===\n";

    HelixSignature256 q_sig{0xAAAAAAAAAAAAAAAAULL, 0x1111111111111111ULL, 0x2222222222222222ULL, 0x3333333333333333ULL};
    std::array<bool, 256> admitted{};
    admitted[10] = true;
    admitted[20] = true;

    // Test 1: Admitted vs non-admitted penalty (+6) and rejected flag filtering
    {
        std::vector<HotRouteEntry> route(3);
        // Cell 0: identical signature, admitted sector 10 -> cost = 0
        route[0] = {{10, false}, q_sig};
        // Cell 1: identical signature, non-admitted sector 11 -> cost = 6
        route[1] = {{11, false}, q_sig};
        // Cell 2: identical signature, admitted sector 10, but rejected = true -> skipped
        route[2] = {{10, true}, q_sig};

        auto res = run_helix_admission_cascade(q_sig, admitted, route);
        assert(res.size() == 2);
        assert(res[0].cell_idx == 0 && res[0].coarse_cost == 0);
        assert(res[1].cell_idx == 1 && res[1].coarse_cost == 6);
        std::cout << "  • Test 1: Admitted bonus (+6 penalty) & REJECT filter ... [PASS]\n";
    }

    // Test 2: Non-admitted cell with sig_ham > 28 is filtered out even if raw192_ham == 0
    {
        std::vector<HotRouteEntry> route(2);
        // Flip 29 bits in s0
        HelixSignature256 sig_29 = q_sig;
        sig_29.s0 ^= ((1ULL << 29) - 1ULL);

        route[0] = {{99, false}, sig_29}; // non-admitted, sig_ham = 29 -> rejected by stage 1
        route[1] = {{10, false}, sig_29}; // admitted, sig_ham = 29 -> cost = 29*4 = 116 <= 212 -> kept

        auto res = run_helix_admission_cascade(q_sig, admitted, route);
        assert(res.size() == 1);
        assert(res[0].cell_idx == 1);
        assert(res[0].coarse_cost == 29 * 4);
        std::cout << "  • Test 2: Stage-1 sig_ham > 28 gate for non-admitted cells ... [PASS]\n";
    }

    // Test 3: Shortlist cap at max_verify = 144 and ordering by (coarse_cost, cell_idx)
    {
        std::vector<HotRouteEntry> route(300);
        for (size_t i = 0; i < 300; ++i) {
            HelixSignature256 s = q_sig;
            // Toggle up to 8 bits in s1 based on (299 - i) % 9
            uint64_t flips = (299 - i) % 9;
            s.s1 ^= ((1ULL << flips) - 1ULL);
            route[i] = {{10, false}, s};
        }
        auto res = run_helix_admission_cascade(q_sig, admitted, route, 144, 212);
        assert(res.size() == 144);
        for (size_t k = 1; k < res.size(); ++k) {
            assert(res[k - 1].coarse_cost < res[k].coarse_cost ||
                   (res[k - 1].coarse_cost == res[k].coarse_cost && res[k - 1].cell_idx < res[k].cell_idx));
        }
        std::cout << "  • Test 3: Shortlist top-144 truncation and monotonic order ... [PASS]\n";
    }

    // Performance benchmark
    std::vector<HotRouteEntry> bench_route(2048);
    for (size_t i = 0; i < bench_route.size(); ++i) {
        uint64_t seed = (i + 1) * 0x9E3779B97F4A7C15ULL;
        bench_route[i] = {
            {static_cast<uint8_t>(i & 255), false},
            {seed, seed ^ (seed >> 17), seed * 3ULL, seed ^ 0x5555555555555555ULL}
        };
    }
    constexpr size_t ITERS = 5000;
    uint64_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        q_sig.s0 ^= i;
        auto out = run_helix_admission_cascade(q_sig, admitted, bench_route);
        if (!out.empty()) checksum += out[0].coarse_cost;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0xFFFFFFFFFFFFULL) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
