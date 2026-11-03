#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 067 Symbolic AST Parser & Evaluator Test Suite ===\n";

    // Test 1: Operator precedence (*, / before +, -) and parentheses override
    {
        double v1 = sym::parse_and_eval("2 + 3 * 4");
        double v2 = sym::parse_and_eval("(2 + 3) * 4");
        assert(std::abs(v1 - 14.0) < 1e-9);
        assert(std::abs(v2 - 20.0) < 1e-9);
        std::cout << "  • Test 1: Operator precedence and parentheses ... [PASS]\n";
    }

    // Test 2: Left-associativity of subtraction and division
    {
        double v_sub = sym::parse_and_eval("10 - 3 - 2");
        double v_div = sym::parse_and_eval("24 / 6 / 2");
        assert(std::abs(v_sub - 5.0) < 1e-9);
        assert(std::abs(v_div - 2.0) < 1e-9);
        std::cout << "  • Test 2: Left-associativity of '-' and '/' ... [PASS]\n";
    }

    // Test 3: Right-associativity of exponentiation (^) and nested subexpressions
    {
        double v_pow = sym::parse_and_eval("2 ^ 3 ^ 2");
        double v_complex = sym::parse_and_eval("((12 + 8) / (2 ^ 2)) * 3 - 1");
        assert(std::abs(v_pow - 512.0) < 1e-9);
        assert(std::abs(v_complex - 14.0) < 1e-9);
        std::cout << "  • Test 3: Right-associative '^' and nested AST evaluation ... [PASS]\n";
    }

    // Performance benchmark
    const std::string bench_expr = "((12 + 8) / (2 ^ 2)) * 3 + (10 - 3 - 2) * (4 + 6 / 2)";
    constexpr size_t ITERS = 25000;
    double sum = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        sum += sym::parse_and_eval(bench_expr);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (sum == -1.0) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
