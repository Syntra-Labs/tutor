#include <cassert>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 074 Hoare Contract Parser & Skeletonizer Test Suite ===\n";

    const std::string sample_src =
        "// @requires: n >= 0\n"
        "// @requires: n <= 1000\n"
        "// @ensures: result == n * (n + 1) / 2\n"
        "int sum_to_n(int n) {\n"
        "    int s = 0;\n"
        "    for (int i = 1; i <= n; ++i) {\n"
        "        if (i > 0) { s += i; }\n"
        "    }\n"
        "    const char* msg = \"ignores { unbalanced } braces in strings\";\n"
        "    (void)msg;\n"
        "    return s;\n"
        "}\n"
        "\n"
        "// @requires: b != 0\n"
        "// @ensures: result * b <= a\n"
        "int safe_div(int a, int b) {\n"
        "    return a / b;\n"
        "}\n";

    // Test 1: Extract @requires and @ensures contracts accurately
    {
        auto specs = extract_hoare_contracts(sample_src);
        assert(specs.size() == 2);
        assert(specs[0].fn_name == "sum_to_n");
        assert(specs[0].requires_clauses.size() == 2);
        assert(specs[0].requires_clauses[0] == "n >= 0");
        assert(specs[0].requires_clauses[1] == "n <= 1000");
        assert(specs[0].ensures_clauses.size() == 1);
        assert(specs[0].ensures_clauses[0] == "result == n * (n + 1) / 2");

        assert(specs[1].fn_name == "safe_div");
        assert(specs[1].requires_clauses.size() == 1);
        assert(specs[1].requires_clauses[0] == "b != 0");
        assert(specs[1].ensures_clauses.size() == 1);
        assert(specs[1].ensures_clauses[0] == "result * b <= a");
        std::cout << "  • Test 1: Hoare @requires/@ensures extraction ... [PASS]\n";
    }

    // Test 2: Skeletonize replaces nested function bodies with { /* TODO */ } and ignores braces in strings
    {
        std::string skel = skeletonize_cpp_source(sample_src);
        assert(skel.find("int sum_to_n(int n) { /* TODO */ }") != std::string::npos);
        assert(skel.find("int safe_div(int a, int b) { /* TODO */ }") != std::string::npos);
        assert(skel.find("// @requires: n >= 0") != std::string::npos);
        assert(skel.find("for (int i = 1;") == std::string::npos);
        assert(skel.find("return a / b;") == std::string::npos);
        std::cout << "  • Test 2: Function body skeletonization with nested braces & strings ... [PASS]\n";
    }

    // Test 3: Contracts on skeletonized output are identical to original contracts
    {
        std::string skel = skeletonize_cpp_source(sample_src);
        auto orig_specs = extract_hoare_contracts(sample_src);
        auto skel_specs = extract_hoare_contracts(skel);
        assert(orig_specs.size() == skel_specs.size());
        for (size_t i = 0; i < orig_specs.size(); ++i) {
            assert(orig_specs[i].fn_name == skel_specs[i].fn_name);
            assert(orig_specs[i].requires_clauses == skel_specs[i].requires_clauses);
            assert(orig_specs[i].ensures_clauses == skel_specs[i].ensures_clauses);
        }
        std::cout << "  • Test 3: Contract invariance under skeletonization ... [PASS]\n";
    }

    // Performance benchmark
    constexpr size_t ITERS = 25000;
    size_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto s = skeletonize_cpp_source(sample_src);
        auto c = extract_hoare_contracts(s);
        checksum += s.size() + c.size();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
