#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 073 Exact Rational Simplex Pivot & Certificate Test Suite ===\n";

    // Test 1: Single Bland's rule pivot on a 2x4 tableau
    {
        // Minimize -3*x0 - 2*x1 subject to:
        //   x0 + x1 + s0 = 4
        //  2x0 + x1 + s1 = 5
        SimplexTableau tab;
        tab.t = {
            {Rational(1), Rational(1), Rational(1), Rational(0)},
            {Rational(2), Rational(1), Rational(0), Rational(1)}
        };
        tab.rhs = {Rational(4), Rational(5)};
        tab.basis = {2, 3};
        tab.d = {Rational(-3), Rational(-2), Rational(0), Rational(0)};

        // Step 1: smallest j with d[j] < 0 is j = 0 (d[0] = -3).
        // Ratios for j=0: row 0 -> 4/1 = 4, row 1 -> 5/2 = 2.5 -> leaves row 1 (basis[1] becomes 0).
        bool pivoted1 = tab.step_blands_rule(4);
        assert(pivoted1 == true);
        assert(tab.basis[1] == 0);
        assert(tab.rhs[1] == Rational(5, 2));
        assert(tab.rhs[0] == Rational(3, 2));

        // Step 2: d[1] = -2 - (-3)*(1/2) = -1/2 < 0 -> enters j = 1, leaves row 0
        bool pivoted2 = tab.step_blands_rule(4);
        assert(pivoted2 == true);
        assert(tab.basis[0] == 1);
        assert(tab.rhs[0] == Rational(3)); // x1 = 3
        assert(tab.rhs[1] == Rational(1)); // x0 = 1

        // Step 3: all reduced costs >= 0 -> optimal!
        bool pivoted3 = tab.step_blands_rule(4);
        assert(pivoted3 == false);
        std::cout << "  • Test 1: Exact rational Bland's-rule simplex pivots ... [PASS]\n";
    }

    // Test 2: Degenerate tie-breaking by smallest basic variable index (Bland's rule)
    {
        SimplexTableau tab;
        tab.t = {
            {Rational(1), Rational(1), Rational(0)},
            {Rational(1), Rational(0), Rational(1)}
        };
        tab.rhs = {Rational(0), Rational(0)}; // Degenerate tie: both ratios are 0/1 = 0
        tab.basis = {5, 2}; // Row 1 has smaller basic variable index (2 < 5)!
        tab.d = {Rational(-1), Rational(0), Rational(0)};

        bool p = tab.step_blands_rule(3);
        assert(p == true);
        assert(tab.basis[1] == 0); // Row 1 must be chosen to leave
        assert(tab.basis[0] == 5);
        std::cout << "  • Test 2: Bland's rule degenerate ratio tie-breaking ... [PASS]\n";
    }

    // Test 3: Primal-dual optimality certificate verification
    {
        std::vector<std::vector<Rational>> A = {
            {Rational(1), Rational(1), Rational(1), Rational(0)},
            {Rational(2), Rational(1), Rational(0), Rational(1)}
        };
        std::vector<Rational> b = {Rational(4), Rational(5)};
        std::vector<Rational> c = {Rational(-3), Rational(-2), Rational(0), Rational(0)};

        // Optimal primal: x = (1, 3, 0, 0), c^T x = -9
        std::vector<Rational> x_opt = {Rational(1), Rational(3), Rational(0), Rational(0)};
        // Optimal dual: y = (-1, -1), b^T y = -4 - 5 = -9, A^T y = (-3, -2, -1, -1) <= c
        std::vector<Rational> y_opt = {Rational(-1), Rational(-1)};

        assert(verify_lp_certificate(A, b, c, x_opt, y_opt) == true);

        // Suboptimal primal x = (0, 4, 0, 1) -> c^T x = -8 != -9 -> rejected!
        std::vector<Rational> x_sub = {Rational(0), Rational(4), Rational(0), Rational(1)};
        assert(verify_lp_certificate(A, b, c, x_sub, y_opt) == false);
        std::cout << "  • Test 3: Exact rational primal-dual certificate verifier ... [PASS]\n";
    }

    // Performance benchmark
    std::vector<std::vector<Rational>> A = {
        {Rational(1), Rational(1), Rational(1), Rational(0)},
        {Rational(2), Rational(1), Rational(0), Rational(1)}
    };
    std::vector<Rational> b = {Rational(4), Rational(5)};
    std::vector<Rational> c = {Rational(-3), Rational(-2), Rational(0), Rational(0)};
    std::vector<Rational> x_opt = {Rational(1), Rational(3), Rational(0), Rational(0)};
    std::vector<Rational> y_opt = {Rational(-1), Rational(-1)};

    constexpr size_t ITERS = 50000;
    size_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        checksum += verify_lp_certificate(A, b, c, x_opt, y_opt) ? 1 : 0;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
