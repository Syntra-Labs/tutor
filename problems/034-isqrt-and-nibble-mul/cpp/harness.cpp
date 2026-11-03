#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 034 Exact Integer Square Root & Base-16 LUT Multiply Suite ===\n";

    // Test 1: isqrt_u64 on small squares, non-squares, and 64-bit boundary cases (> 2^53 where double fails)
    assert(isqrt_u64(0) == 0);
    assert(isqrt_u64(1) == 1);
    assert(isqrt_u64(2) == 1);
    assert(isqrt_u64(3) == 1);
    assert(isqrt_u64(4) == 2);
    assert(isqrt_u64(15) == 3);
    assert(isqrt_u64(16) == 4);
    // Max uint64_t = 2^64 - 1 -> floor(sqrt(2^64 - 1)) == 2^32 - 1 == 4294967295
    // Note: std::sqrt(static_cast<double>(~0ULL)) rounds up to 4294967296.0!
    assert(isqrt_u64(~0ULL) == 0xFFFFFFFFULL);
    uint64_t big_sq = 0xFFFFFFFE00000001ULL; // (2^32 - 1)^2
    assert(isqrt_u64(big_sq - 1) == 0xFFFFFFFEULL);
    assert(isqrt_u64(big_sq) == 0xFFFFFFFFULL);
    std::cout << "  • Test 1: Exact 64-bit isqrt_u64 (including > 2^53 boundary) ... [PASS]\n";

    // Test 2: lut_mul_u64 exact parity with 64-bit hardware multiplication
    assert(lut_mul_u64(0, 123456789u) == 0ULL);
    assert(lut_mul_u64(123456789u, 0) == 0ULL);
    assert(lut_mul_u64(0xFFFFFFFFu, 0xFFFFFFFFu) == static_cast<uint64_t>(0xFFFFFFFFu) * 0xFFFFFFFFu);
    assert(lut_mul_u64(314159265u, 271828183u) == 314159265ULL * 271828183ULL);
    std::cout << "  • Test 2: Base-16 multiples-table product lut_mul_u64 parity ... [PASS]\n";

    // Test 3: Commutativity & distributivity of lut_mul_u64
    uint32_t x = 0x13579BDFu, y = 0x2468ACE0u, z = 0x0F0F0F0Fu;
    assert(lut_mul_u64(x, y) == lut_mul_u64(y, x));
    assert(lut_mul_u64(x, y + z) == lut_mul_u64(x, y) + lut_mul_u64(x, z));
    std::cout << "  • Test 3: lut_mul_u64 commutativity & distributivity ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 1000000;
    volatile uint64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (uint32_t i = 1; i <= ITERS; ++i) {
        sink ^= isqrt_u64(lut_mul_u64(i * 2654435761u, i));
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 034 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
