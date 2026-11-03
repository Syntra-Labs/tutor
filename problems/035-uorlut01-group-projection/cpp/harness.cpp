#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 035 UORLUT01 4-Bit Group-Scaled LUT Projection Suite ===\n";

    // Test 1: ActivationLUT construction for positive, negative, and zero activations
    int32_t x_test[2] = {13, -7};
    ActivationLUT luts[2]{};
    build_activation_luts(x_test, 2, luts);
    for (int u = 0; u < 16; ++u) {
        assert(luts[0][u] == static_cast<int64_t>(u - 8) * 13);
        assert(luts[1][u] == static_cast<int64_t>(u - 8) * (-7));
    }
    std::cout << "  • Test 1: ActivationLUT table construction [-8..7] * x_j ... [PASS]\n";

    // Test 2: Zero-code (u = 8 -> q = 0) produces exact zero output regardless of scales
    constexpr size_t D_OUT = 4;
    constexpr size_t D_IN  = 64; // 2 groups of 32
    std::vector<uint8_t> zero_weights(D_OUT * (D_IN / 2), 0x88u); // both nibbles = 8 (q = 0)
    std::vector<uint8_t> scales(D_OUT * (D_IN / 32), 0x35u);      // m = 5, e = 3 (scale = 40)
    std::vector<int32_t> x(D_IN, 42);
    std::vector<int64_t> y(D_OUT, -1);

    uorlut01_project(zero_weights.data(), scales.data(), x.data(), D_OUT, D_IN, y.data());
    for (size_t i = 0; i < D_OUT; ++i) {
        assert(y[i] == 0);
    }
    std::cout << "  • Test 2: Zero-quantized weight code (0x88) yields exact zero ... [PASS]\n";

    // Test 3: Exact bit-for-bit parity against reference integer dot product across multiple groups & rows
    std::vector<uint8_t> packed_w(D_OUT * (D_IN / 2));
    for (size_t idx = 0; idx < packed_w.size(); ++idx) {
        uint8_t u0 = static_cast<uint8_t>((idx * 7 + 3) & 0x0F);
        uint8_t u1 = static_cast<uint8_t>((idx * 11 + 5) & 0x0F);
        packed_w[idx] = static_cast<uint8_t>(u0 | (u1 << 4));
    }
    for (size_t idx = 0; idx < scales.size(); ++idx) {
        uint8_t m = static_cast<uint8_t>((idx * 3 + 1) & 0x0F);
        uint8_t e = static_cast<uint8_t>((idx * 2) & 0x07);
        scales[idx] = static_cast<uint8_t>(m | (e << 4));
    }
    for (size_t j = 0; j < D_IN; ++j) {
        x[j] = static_cast<int32_t>((j * 17) - 500);
    }

    uorlut01_project(packed_w.data(), scales.data(), x.data(), D_OUT, D_IN, y.data());

    for (size_t i = 0; i < D_OUT; ++i) {
        int64_t ref_sum = 0;
        for (size_t g = 0; g < D_IN / 32; ++g) {
            uint8_t sc = scales[i * (D_IN / 32) + g];
            int64_t s_val = static_cast<int64_t>(sc & 0x0F) << ((sc >> 4) & 0x0F);
            int64_t g_sum = 0;
            for (size_t c = 0; c < 32; ++c) {
                size_t col = g * 32 + c;
                uint8_t byte = packed_w[i * (D_IN / 2) + (col / 2)];
                uint8_t u = (col % 2 == 0) ? (byte & 0x0F) : ((byte >> 4) & 0x0F);
                int64_t q = static_cast<int64_t>(u) - 8;
                g_sum += q * static_cast<int64_t>(x[col]);
            }
            ref_sum += g_sum * s_val;
        }
        assert(y[i] == ref_sum);
    }
    std::cout << "  • Test 3: Bit-exact parity against reference group-scaled dot product ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 100000;
    volatile int64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        uorlut01_project(packed_w.data(), scales.data(), x.data(), D_OUT, D_IN, y.data());
        sink ^= y[0];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 035 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
