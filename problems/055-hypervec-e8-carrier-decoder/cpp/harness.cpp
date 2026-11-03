#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

// Helper to set a 4-word (256-bit) slice to have a specified number of 1-bits
static void set_slice_popcount(Hypervec8192& hv, size_t b, size_t d, uint32_t ones) {
    size_t base_w = b * 32 + d * 4;
    for (size_t k = 0; k < 4; ++k) {
        if (ones >= 64) {
            hv.words[base_w + k] = ~0ULL;
            ones -= 64;
        } else if (ones > 0) {
            hv.words[base_w + k] = (1ULL << ones) - 1ULL;
            ones = 0;
        } else {
            hv.words[base_w + k] = 0ULL;
        }
    }
}

int main() {
    std::cout << "=== Running 055 Hypervector-to-E8 Carrier Decoder Test Suite ===\n";

    // Test 1: Balanced hypervector (128 ones per 256-bit slice) -> all coordinates 0, roots = 0
    Hypervec8192 hv_bal{};
    for (size_t b = 0; b < 4; ++b)
        for (size_t d = 0; d < 8; ++d)
            set_slice_popcount(hv_bal, b, d, 128);

    E8CarrierAddress addr0 = decode_e8_carrier(hv_bal);
    for (int d = 0; d < 8; ++d) {
        assert(addr0.first_block[d] == 0 && addr0.last_block[d] == 0);
    }
    assert(addr0.admission_cell == 0);
    std::cout << "  • Test 1: Balanced 50%-density hypervector decodes to zero coordinates ... [PASS]\n";

    // Test 2: Encode block 0 as root 10 (0..0, 2, 2) and block 3 as root 5 (0, 0, 0, 0, -2, -2, 0, 0)
    // Coordinate +2 requires ones = 128 + 12 = 140; coordinate -2 requires ones = 128 - 12 = 116
    Hypervec8192 hv_target = hv_bal;
    set_slice_popcount(hv_target, 0, 6, 140);
    set_slice_popcount(hv_target, 0, 7, 140);
    set_slice_popcount(hv_target, 3, 4, 116);
    set_slice_popcount(hv_target, 3, 5, 116);

    E8CarrierAddress addr1 = decode_e8_carrier(hv_target);
    assert(addr1.first_block[6] == 2 && addr1.first_block[7] == 2);
    assert(addr1.last_block[4] == -2 && addr1.last_block[5] == -2);
    assert(addr1.block_roots[0] == 10);
    assert(addr1.block_roots[3] == 5);
    assert(addr1.admission_cell == ((10 << 4) | 5));
    std::cout << "  • Test 2: Exact block root recovery and 256-cell packing ((r0 << 4) | r3) ... [PASS]\n";

    // Test 3: All-ones hypervector (256 ones -> (256-128)/6 = +21 on every coordinate)
    Hypervec8192 hv_ones{};
    hv_ones.words.fill(~0ULL);
    E8CarrierAddress addr2 = decode_e8_carrier(hv_ones);
    for (int d = 0; d < 8; ++d) {
        assert(addr2.first_block[d] == 21 && addr2.last_block[d] == 21);
    }
    assert(addr2.block_roots[0] == 0 && addr2.block_roots[3] == 0);
    std::cout << "  • Test 3: Saturation coordinate scaling ((256 - 128) / 6 == 21) ... [PASS]\n";

    constexpr int ITERS = 100000;
    volatile uint8_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        hv_target.words[0] ^= static_cast<uint64_t>(i);
        auto a = decode_e8_carrier(hv_target);
        sink = sink + a.admission_cell;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
