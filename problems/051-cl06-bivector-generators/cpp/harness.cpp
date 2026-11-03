#include <cassert>
#include <chrono>
#include <iostream>
#include <set>

#include "solution.hpp"

static Mat8 mat_neg(const Mat8& M) {
    Mat8 out{};
    for (size_t r = 0; r < 8; ++r)
        for (size_t c = 0; c < 8; ++c)
            out[r][c] = static_cast<int8_t>(-M[r][c]);
    return out;
}

int main() {
    std::cout << "=== Running 051 Clifford Algebra Cl(0,6) & 15 Bivectors Test Suite ===\n";

    Mat8 I8 = mat_identity();
    Mat8 neg_I8 = mat_neg(I8);
    auto gens = build_cl06_generators();
    auto bivs = build_cl06_bivectors();

    // Test 1: Clifford generator relations G_i^2 = -I_8 and G_i G_j = -G_j G_i (i != j)
    for (size_t i = 0; i < 6; ++i) {
        assert(mat_mul(gens[i], gens[i]) == neg_I8);
        for (size_t j = i + 1; j < 6; ++j) {
            assert(mat_mul(gens[i], gens[j]) == mat_neg(mat_mul(gens[j], gens[i])));
        }
    }
    std::cout << "  • Test 1: Cl(0,6) anticommutation relations G_i^2 = -I_8, G_i G_j = -G_j G_i ... [PASS]\n";

    // Test 2: All 15 bivectors are distinct and satisfy B_m^2 = -I_8
    std::set<Mat8> unique_bivs(bivs.begin(), bivs.end());
    assert(unique_bivs.size() == 15);
    for (const auto& B : bivs) {
        assert(mat_mul(B, B) == neg_I8);
    }
    std::cout << "  • Test 2: 15 distinct bivectors with B_m^2 = -I_8 ... [PASS]\n";

    // Test 3: K_6 path composition B_01 * B_12 == -B_02
    // B_01 is bivs[0] (0,1), B_02 is bivs[1] (0,2), B_12 is bivs[5] (1,2)
    assert(mat_mul(bivs[0], bivs[5]) == mat_neg(bivs[1]));
    std::cout << "  • Test 3: K_6 edge path composition B_ij B_jk = -B_ik ... [PASS]\n";

    constexpr int ITERS = 50000;
    volatile int sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        auto C = mat_mul(bivs[i % 15], bivs[(i + 3) % 15]);
        sink = sink + C[0][0];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
