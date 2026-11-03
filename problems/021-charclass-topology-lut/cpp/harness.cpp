#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string_view>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 021 Character-Class Topology & Transition Flux Suite ===\n";

    // Test 1: Exact LUT classification across all 7 classes
    assert(classify_byte(' ') == CharClass::SPACE);
    assert(classify_byte('\n') == CharClass::SPACE);
    assert(classify_byte('a') == CharClass::LOWER);
    assert(classify_byte('z') == CharClass::LOWER);
    assert(classify_byte('A') == CharClass::UPPER);
    assert(classify_byte('Z') == CharClass::UPPER);
    assert(classify_byte('0') == CharClass::DIGIT);
    assert(classify_byte('9') == CharClass::DIGIT);
    assert(classify_byte('+') == CharClass::OP_MATH);
    assert(classify_byte('\\') == CharClass::OP_MATH);
    assert(classify_byte('(') == CharClass::DELIM);
    assert(classify_byte('`') == CharClass::DELIM);
    assert(classify_byte(0xFF) == CharClass::OTHER);
    std::cout << "  • Test 1: Constexpr 7-class LUT classification ... [PASS]\n";

    // Test 2: Zero transition flux on uniform character class
    auto st_uniform = compute_topology_flux("abcdefghij");
    assert(std::abs(st_uniform.transition_flux - 0.0) < 1e-9);
    assert(std::abs(st_uniform.op_density - 0.0) < 1e-9);
    assert(std::abs(st_uniform.delim_density - 0.0) < 1e-9);
    std::cout << "  • Test 2: Uniform class zero transition flux ... [PASS]\n";

    // Test 3: Known transition flux and operator/delimiter density on "a+1(b)"
    // Classes: 'a'(LOWER), '+'(OP_MATH), '1'(DIGIT), '('(DELIM), 'b'(LOWER), ')'(DELIM)
    // All 5 adjacent pairs change class -> transitions = 5, sampled = 6 -> flux = 5/6
    // op_count = 1 ('+') -> 1/6, delim_count = 2 ('(', ')') -> 2/6
    auto st_mixed = compute_topology_flux("a+1(b)");
    assert(std::abs(st_mixed.transition_flux - (5.0 / 6.0)) < 1e-9);
    assert(std::abs(st_mixed.op_density - (1.0 / 6.0)) < 1e-9);
    assert(std::abs(st_mixed.delim_density - (2.0 / 6.0)) < 1e-9);
    std::cout << "  • Test 3: Mixed expression flux & densities ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 100000;
    std::string_view bench_str = "for (size_t i = 0; i < N; ++i) { sum += data[i] * 2; }";
    volatile double sink = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto s = compute_topology_flux(bench_str);
        sink += s.transition_flux;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 021 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
