#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 031 Bit-Exact IEEE-754 binary32 to Dyadic Decoder Suite ===\n";

    // Test 1: NaN and +/-Infinity rejection, and +/-0.0 canonicalization
    assert(!decode_f32_bits(0x7F800000u).has_value()); // +Inf
    assert(!decode_f32_bits(0xFF800000u).has_value()); // -Inf
    assert(!decode_f32_bits(0x7FC00001u).has_value()); // NaN
    assert(decode_f32_bits(0x00000000u) == (ExactDyadicF32{0, 0})); // +0.0f
    assert(decode_f32_bits(0x80000000u) == (ExactDyadicF32{0, 0})); // -0.0f
    std::cout << "  • Test 1: NaN/Inf rejection & signed zero canonicalization ... [PASS]\n";

    // Test 2: Exact normal binary32 values (1.0f, -1.75f, 0.375f)
    // 1.0f = 0x3F800000 -> 1 * 2^0
    assert(decode_f32_bits(0x3F800000u) == (ExactDyadicF32{1, 0}));
    // -1.75f = -7/4 = -7 * 2^-2 -> 0xBFE00000
    assert(decode_f32_bits(0xBFE00000u) == (ExactDyadicF32{-7, -2}));
    // 0.375f = 3/8 = 3 * 2^-3 -> 0x3EC00000
    assert(decode_f32_bits(0x3EC00000u) == (ExactDyadicF32{3, -3}));
    std::cout << "  • Test 2: Normal floats reduced to canonical odd mantissa ... [PASS]\n";

    // Test 3: Subnormal binary32 values (min positive subnormal 0x00000001 = 1 * 2^-149, and 0x00000006 = 3 * 2^-148)
    assert(decode_f32_bits(0x00000001u) == (ExactDyadicF32{1, -149}));
    assert(decode_f32_bits(0x00000006u) == (ExactDyadicF32{3, -148}));
    assert(decode_f32_bits(0x80000006u) == (ExactDyadicF32{-3, -148}));
    std::cout << "  • Test 3: Subnormal binary32 exact exponent (-149) & reduction ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 1000000;
    volatile int64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (uint32_t i = 0; i < ITERS; ++i) {
        auto d = decode_f32_bits(0x3F800000u + (i & 0x7FFFFFu));
        sink ^= d->m;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 031 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
