#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 029 Z[phi] Galois Conjugate, Norm & Unit Inversion Suite ===\n";

    constexpr ZPhi one{1, 0};
    constexpr ZPhi phi{0, 1};

    // Test 1: Galois conjugate involution & homomorphism
    ZPhi x{4, -3}, y{-2, 5};
    assert(x.conj().conj() == x);
    assert((x * y).conj() == (x.conj() * y.conj()));
    assert((x * x.conj()) == (ZPhi{x.norm(), 0}));
    std::cout << "  • Test 1: Galois conjugate involution & ring homomorphism ... [PASS]\n";

    // Test 2: Multiplicativity of norm N(x * y) == N(x) * N(y)
    assert((x * y).norm() == x.norm() * y.norm());
    assert(phi.norm() == -1);
    assert((phi * phi).norm() == 1);
    std::cout << "  • Test 2: Norm multiplicativity N(xy) == N(x)N(y) ... [PASS]\n";

    // Test 3: Unit inversion for N(u) == +-1 and rejection when |N(u)| != 1
    auto inv_phi = phi.inv();
    assert(inv_phi.has_value());
    assert(*inv_phi == (ZPhi{-1, 1}));
    assert((phi * (*inv_phi)) == one);

    ZPhi u{2, 3}; // 2 + 3*phi -> norm = 4 + 6 - 9 = 1
    assert(u.norm() == 1);
    auto inv_u = u.inv();
    assert(inv_u.has_value());
    assert((u * (*inv_u)) == one);

    ZPhi non_unit{2, 0}; // norm = 4
    assert(!non_unit.inv().has_value());
    std::cout << "  • Test 3: Exact unit inversion & non-unit rejection ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 1000000;
    volatile int64_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        ZPhi z{static_cast<int64_t>(i & 255), static_cast<int64_t>((i >> 4) & 255)};
        sink ^= z.norm();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 029 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
