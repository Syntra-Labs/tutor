#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 024 Order-2 Interpolated Dirichlet Byte-Markov Suite ===\n";

    ByteMarkovModel model{};

    // Test 1: Unobserved uniform Dirichlet prior yields exactly 8.0 bits of surprisal (P = 1/256)
    double s0 = model.byte_surprisal('a', 'b');
    assert(std::abs(s0 - 8.0) < 1e-9);
    std::cout << "  • Test 1: Cold-start uniform prior surprisal == 8.0 bits ... [PASS]\n";

    // Test 2: Repeated bigram lowers surprisal monotonically while rare bigram increases surprisal
    std::string corpus;
    for (int i = 0; i < 500; ++i) corpus += "ab";
    model.observe_stream(corpus);

    double s_ab = model.byte_surprisal('a', 'b');
    double s_az = model.byte_surprisal('a', 'z');
    assert(s_ab < 1.0);
    assert(s_az > 8.0);
    std::cout << "  • Test 2: High-frequency transition ('a'->'b') < 1 bit vs rare ('a'->'z') > 8 bits ... [PASS]\n";

    // Test 3: Partition of unity sum_{cur=0..255} 2^{-byte_surprisal(prev, cur)} == 1.0
    double prob_sum = 0.0;
    for (int c = 0; c < 256; ++c) {
        double s = model.byte_surprisal('a', static_cast<uint8_t>(c));
        prob_sum += std::exp2(-s);
    }
    assert(std::abs(prob_sum - 1.0) < 1e-9);
    std::cout << "  • Test 3: Conditional probability distribution sums to 1.0 ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 1000000;
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        sink += model.byte_surprisal(static_cast<uint8_t>(i & 0xFF), static_cast<uint8_t>((i >> 8) & 0xFF));
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 024 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
