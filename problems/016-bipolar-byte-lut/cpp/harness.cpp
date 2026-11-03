#include <array>
#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 016 Precomputed 256x8 Bipolar Byte LUT Test Suite ===\n";

    // Test 1: Alignment and antipodal byte symmetry table[b][k] + table[255 - b][k] == 0
    assert(reinterpret_cast<uintptr_t>(&BYTE_BIPOLAR_LUT.table[0][0]) % 32 == 0);
    for (int b = 0; b < 256; ++b) {
        int row_sum = 0;
        for (int k = 0; k < 8; ++k) {
            int16_t v = BYTE_BIPOLAR_LUT.table[b][k];
            int16_t v_inv = BYTE_BIPOLAR_LUT.table[255 - b][k];
            assert(v == 1 || v == -1);
            assert(v + v_inv == 0);
            row_sum += v;
        }
        assert(row_sum == 2 * std::popcount(static_cast<uint8_t>(b)) - 8);
    }
    std::cout << "  • Test 1: 32-byte alignment, antipodal symmetry & row popcount identity ... [PASS]\n";

    // Test 2: Compile-time constexpr evaluation check
    static_assert(BYTE_BIPOLAR_LUT.table[0x00][0] == -1 || BYTE_BIPOLAR_LUT.table[0x00][0] == 0);
    assert(BYTE_BIPOLAR_LUT.table[0x00][0] == -1);
    assert(BYTE_BIPOLAR_LUT.table[0xFF][7] == 1);
    assert(BYTE_BIPOLAR_LUT.table[0xAA][0] == -1 && BYTE_BIPOLAR_LUT.table[0xAA][1] == 1);
    std::cout << "  • Test 2: Constexpr boundary & alternating bit patterns ... [PASS]\n";

    // Test 3: 64-bit word expansion matches bit-by-bit reference
    uint64_t test_word = 0xF0E1D2C3B4A59687ULL;
    std::array<int16_t, 64> expanded{};
    expand_word_bipolar(test_word, expanded.data());
    int sum64 = 0;
    for (int i = 0; i < 64; ++i) {
        int16_t expected = ((test_word >> i) & 1ULL) ? 1 : -1;
        assert(expanded[i] == expected);
        sum64 += expanded[i];
    }
    assert(sum64 == 2 * std::popcount(test_word) - 64);
    std::cout << "  • Test 3: Full 64-bit word expansion exactness ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 1000000;
    alignas(32) std::array<int16_t, 64> buf{};
    int64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        expand_word_bipolar(test_word ^ i, buf.data());
        sink += buf[i & 63];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (sink == 0xdeadbeefLL) std::cout << sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 016 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
