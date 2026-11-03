#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 014 VSA XOR Binding & Unbinding Test Suite ===\n";

    Hypervec8192 A = Hypervec8192::from_seed(1001ULL);
    Hypervec8192 B = Hypervec8192::from_seed(2002ULL);
    Hypervec8192 C = Hypervec8192::from_seed(3003ULL);

    // Test 1: Self-inverse unbinding (A ^ B) ^ B == A and nilpotency A ^ A == 0
    Hypervec8192 AB = A.bind(B);
    Hypervec8192 recovered_A = AB.bind(B);
    assert(recovered_A.hamming(A) == 0u);

    Hypervec8192 AA = A.bind(A);
    for (uint64_t w : AA.words) assert(w == 0ULL);
    std::cout << "  • Test 1: Exact self-inverse unbinding (A ^ B) ^ B == A ... [PASS]\n";

    // Test 2: Distance preservation isometry d_H(A ^ C, B ^ C) == d_H(A, B) & orthogonality to inputs
    assert(A.bind(C).hamming(B.bind(C)) == A.hamming(B));
    assert(std::abs(AB.z_score(A)) < 3.5);
    assert(std::abs(AB.z_score(B)) < 3.5);
    std::cout << "  • Test 2: Hamming isometry & operand quasi-orthogonality ... [PASS]\n";

    // Test 3: Role-filler unbinding from noisy bundled trace
    std::array<Hypervec8192, 8> fillers{};
    for (size_t i = 0; i < 8; ++i) fillers[i] = Hypervec8192::from_seed(5000ULL + i);
    Hypervec8192 role_x = Hypervec8192::from_seed(777ULL);
    Hypervec8192 role_y = Hypervec8192::from_seed(888ULL);
    Hypervec8192 role_z = Hypervec8192::from_seed(999ULL);

    // Majority bundle of 3 bound pairs: maj(rx^f[3], ry^f[5], rz^f[1])
    Hypervec8192 p0 = role_x.bind(fillers[3]);
    Hypervec8192 p1 = role_y.bind(fillers[5]);
    Hypervec8192 p2 = role_z.bind(fillers[1]);
    Hypervec8192 trace{};
    for (size_t w = 0; w < Hypervec8192::HV_WORDS; ++w) {
        trace.words[w] = (p0.words[w] & p1.words[w]) |
                         (p1.words[w] & p2.words[w]) |
                         (p0.words[w] & p2.words[w]);
    }

    assert(unbind_and_match(trace, role_x, fillers) == 3u);
    assert(unbind_and_match(trace, role_y, fillers) == 5u);
    assert(unbind_and_match(trace, role_z, fillers) == 1u);
    std::cout << "  • Test 3: Noisy 3-pair superimposed trace unbinding & cleanup ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 100000;
    Hypervec8192 acc = A;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        acc = acc.bind(B);
        acc.words[i & 127] ^= i;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (acc.words[0] == 0xdeadbeefULL) std::cout << acc.words[0];
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 014 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
