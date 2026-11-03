#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 022 Empirical Byte Shannon Entropy & Q8 Quantizer Suite ===\n";

    // Test 1: Constant byte stream has 0 bits of entropy
    assert(std::abs(compute_shannon_entropy("") - 0.0) < 1e-9);
    assert(std::abs(compute_shannon_entropy("aaaaaaaaaaaaaaaa") - 0.0) < 1e-9);
    assert(compute_entropy_q8("aaaaaaaaaaaaaaaa") == 0);
    std::cout << "  • Test 1: Constant stream H = 0.0, q8 = 0 ... [PASS]\n";

    // Test 2: Equiprobable 2-symbol and 4-symbol alphabets
    assert(std::abs(compute_shannon_entropy("abababab") - 1.0) < 1e-9);
    assert(compute_entropy_q8("abababab") == 32);
    assert(std::abs(compute_shannon_entropy("abcdabcdabcdabcd") - 2.0) < 1e-9);
    assert(compute_entropy_q8("abcdabcdabcdabcd") == 64);
    std::cout << "  • Test 2: Dyadic equiprobable 2-symbol (1 bit) & 4-symbol (2 bits) ... [PASS]\n";

    // Test 3: Maximum 256-symbol uniform distribution -> H = 8.0 bits, q8 = 255
    std::string all256(256, '\0');
    for (int i = 0; i < 256; ++i) all256[i] = static_cast<char>(i);
    double h256 = compute_shannon_entropy(all256);
    assert(std::abs(h256 - 8.0) < 1e-9);
    assert(compute_entropy_q8(all256) == 255);
    std::cout << "  • Test 3: Uniform 256-byte distribution H = 8.0, q8 = 255 ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 100000;
    volatile uint8_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        sink ^= compute_entropy_q8(all256);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 022 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
