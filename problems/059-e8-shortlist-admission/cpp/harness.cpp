#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 059 E8 Shortlist Admission Test Suite ===\n";

    // Test 1: Exact root match admits home cell with cost 0 and exactly 32 cells total
    {
        E8CarrierAddress q{};
        for (int d = 0; d < 8; ++d) {
            q.first_block[d] = E8_ORBIT_ROOTS[3][d];
            q.last_block[d]  = E8_ORBIT_ROOTS[12][d];
        }
        q.block_roots[0] = 3;
        q.block_roots[3] = 12;
        q.admission_cell = static_cast<uint8_t>((3 << 4) | 12);

        std::array<uint32_t, 256> costs{};
        auto mask = compute_e8_shortlist_admission(q, &costs);

        size_t count = 0;
        for (bool b : mask) if (b) ++count;
        assert(count == 32);
        assert(costs[q.admission_cell] == 0);
        assert(mask[q.admission_cell] == true);
        std::cout << "  • Test 1: Home cell cost 0 and K=32 cardinality ... [PASS]\n";
    }

    // Test 2: Zero query blocks — verify deterministic lexicographic tie-breaking (cost, cell_id)
    {
        E8CarrierAddress q{};
        std::array<uint32_t, 256> costs{};
        auto mask = compute_e8_shortlist_admission(q, &costs);

        size_t count = 0;
        for (bool b : mask) if (b) ++count;
        assert(count == 32);

        // Verify order invariant: every admitted cell (a) and non-admitted cell (b) satisfies
        // costs[a] < costs[b] || (costs[a] == costs[b] && a < b)
        for (size_t a = 0; a < 256; ++a) {
            if (!mask[a]) continue;
            for (size_t b = 0; b < 256; ++b) {
                if (mask[b]) continue;
                assert(costs[a] < costs[b] || (costs[a] == costs[b] && a < b));
            }
        }
        std::cout << "  • Test 2: Monotonicity and tie-breaking invariant ... [PASS]\n";
    }

    // Test 3: Antipodal roots placed at maximum L1 cost are rejected
    {
        E8CarrierAddress q{};
        for (int d = 0; d < 8; ++d) {
            q.first_block[d] = E8_ORBIT_ROOTS[12][d]; // all +1
            q.last_block[d]  = E8_ORBIT_ROOTS[12][d]; // all +1
        }
        std::array<uint32_t, 256> costs{};
        auto mask = compute_e8_shortlist_admission(q, &costs);

        uint8_t home_cell = static_cast<uint8_t>((12 << 4) | 12);
        uint8_t antipodal_cell = static_cast<uint8_t>((15 << 4) | 15); // all -1
        assert(mask[home_cell] == true);
        assert(mask[antipodal_cell] == false);
        assert(costs[antipodal_cell] == 32);
        std::cout << "  • Test 3: Antipodal cell rejection verified ... [PASS]\n";
    }

    // Performance benchmark loop
    E8CarrierAddress bench_q{};
    for (int d = 0; d < 8; ++d) {
        bench_q.first_block[d] = (d % 3) - 1;
        bench_q.last_block[d] = ((d + 1) % 3) - 1;
    }
    constexpr size_t ITERS = 50000;
    uint64_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        bench_q.first_block[i & 7] ^= 1;
        auto m = compute_e8_shortlist_admission(bench_q, nullptr);
        checksum += m[(i * 7) & 255];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0xFFFFFFFFFFFFULL) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
