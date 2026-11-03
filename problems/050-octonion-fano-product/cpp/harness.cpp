#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 050 Octonion Fano Plane Basis Product Test Suite ===\n";

    // Test 1: Identity e_0 and squares e_i^2 = -e_0
    for (uint8_t i = 0; i < 8; ++i) {
        assert(octonion_basis_product(0, i) == std::make_pair(int8_t(1), i));
        assert(octonion_basis_product(i, 0) == std::make_pair(int8_t(1), i));
        if (i > 0) {
            assert(octonion_basis_product(i, i) == std::make_pair(int8_t(-1), uint8_t(0)));
        }
    }
    std::cout << "  • Test 1: Identity e_0 and imaginary square relations e_i^2 = -e_0 ... [PASS]\n";

    // Test 2: Cyclic Fano line orientation and anti-commutativity
    for (const auto& cyc : FANO_CYCLES) {
        uint8_t a = cyc[0], b = cyc[1], c = cyc[2];
        assert(octonion_basis_product(a, b) == std::make_pair(int8_t(1), c));
        assert(octonion_basis_product(b, a) == std::make_pair(int8_t(-1), c));
        assert(octonion_basis_product(b, c) == std::make_pair(int8_t(1), a));
        assert(octonion_basis_product(c, a) == std::make_pair(int8_t(1), b));
    }
    std::cout << "  • Test 2: All 7 oriented Fano cycles and anti-commutativity ... [PASS]\n";

    // Test 3: 168 of 210 ordered distinct imaginary triples are anti-associative
    assert(count_anti_associative_triples() == 168);
    std::cout << "  • Test 3: 168/210 non-collinear triples are anti-associative ... [PASS]\n";

    constexpr int ITERS = 200000;
    volatile int sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        auto [sgn, idx] = octonion_basis_product(i & 7, (i >> 3) & 7);
        sink = sink + sgn + idx;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
