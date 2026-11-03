#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 036 Prime Token Factor Set & GCD Intersection Suite ===\n";

    // Test 1: Worked example from primes-and-zeta.md: 42 = 2*3*7 (tokens 0,1,3) & 105 = 3*5*7 (tokens 1,2,3) -> gcd = 21 (tokens 1,3)
    uint64_t pA = encode_prime_set({0, 1, 3});
    uint64_t pB = encode_prime_set({1, 2, 3});
    assert(pA == 42ULL);
    assert(pB == 105ULL);
    assert(std::gcd(pA, pB) == 21ULL);
    auto shared = intersect_prime_sets(pA, pB);
    assert((shared == std::vector<size_t>{1, 3}));
    std::cout << "  • Test 1: Worked example gcd(42, 105) == 21 -> tokens {1, 3} ... [PASS]\n";

    // Test 2: Idempotence under duplicate tokens in input (square-free encoding)
    uint64_t p_dup = encode_prime_set({3, 1, 3, 0, 1});
    assert(p_dup == 42ULL);
    std::cout << "  • Test 2: Square-free idempotence on duplicate token IDs ... [PASS]\n";

    // Test 3: Disjoint sets yield gcd == 1 (empty intersection) and full 15-prime primorial
    uint64_t p_even = encode_prime_set({0, 2, 4, 6, 8, 10, 12, 14});
    uint64_t p_odd  = encode_prime_set({1, 3, 5, 7, 9, 11, 13});
    assert(intersect_prime_sets(p_even, p_odd).empty());
    auto all_15 = intersect_prime_sets(p_even * p_odd, p_even * p_odd);
    assert(all_15.size() == 15);
    for (size_t k = 0; k < 15; ++k) assert(all_15[k] == k);
    std::cout << "  • Test 3: Disjoint sets & 15-prime primorial exact recovery ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 500000;
    volatile size_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto v = intersect_prime_sets(p_even * 3 * 7, p_odd * 2);
        sink += v.size();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 036 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
