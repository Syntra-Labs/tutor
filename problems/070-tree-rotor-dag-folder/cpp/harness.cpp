#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 070 Non-Commutative Tree Rotor DAG Folder Test Suite ===\n";

    TreeRotorOperator op;

    // Test 1: Non-commutative asymmetry d_S3(fold(APP(a, b)), fold(APP(b, a))) > 0
    {
        ExprNode a{1, ExprKind::CONST, 0, "Nat.sub", {}};
        ExprNode b{2, ExprKind::BVAR, 0, "", {}};
        double asym = op.non_commutative_asymmetry(a, b);
        assert(asym > 0.05);
        std::cout << "  • Test 1: Non-commutative branch asymmetry d_S3 = " << asym << " ... [PASS]\n";
    }

    // Test 2: Zero sliding-window truncation on a 63-node depth-6 binary tree
    {
        std::vector<ExprNode> nodes;
        for (uint32_t i = 0; i < 32; ++i) {
            nodes.push_back({i, ExprKind::CONST, i, "c" + std::to_string(i), {}});
        }
        uint32_t prev_start = 0;
        uint32_t prev_count = 32;
        uint32_t curr_id = 32;
        while (prev_count > 1) {
            uint32_t next_count = prev_count / 2;
            for (uint32_t j = 0; j < next_count; ++j) {
                nodes.push_back({curr_id + j, ExprKind::APP, 0, "", {prev_start + 2 * j, prev_start + 2 * j + 1}});
            }
            prev_start = curr_id;
            prev_count = next_count;
            curr_id += next_count;
        }
        size_t visited = 0;
        Quat q = op.fold_dag(nodes, nodes.back().id, &visited);
        assert(visited == 63);
        assert(std::abs(q.norm() - 1.0) < 1e-9);
        std::cout << "  • Test 2: Zero truncation on 63-node tree & unit S^3 norm ... [PASS]\n";
    }

    // Test 3: Diamond DAG memoization visits shared nodes exactly once
    {
        // Node 1 is shared by both children of Node 2, and Node 2 is shared by both children of Node 3
        std::vector<ExprNode> diamond = {
            {1, ExprKind::LIT, 42, "", {}},
            {2, ExprKind::LAM, 0, "", {1, 1}},
            {3, ExprKind::FORALLE, 0, "", {2, 2}},
            {4, ExprKind::LETE, 0, "", {1, 2, 3}}
        };
        size_t visited = 0;
        Quat q = op.fold_dag(diamond, 4, &visited);
        assert(visited == 4);
        assert(std::abs(q.norm() - 1.0) < 1e-9);
        std::cout << "  • Test 3: Diamond DAG memoization (4 unique nodes) ... [PASS]\n";
    }

    // Performance benchmark
    std::vector<ExprNode> bench_nodes;
    for (uint32_t i = 0; i < 16; ++i) {
        bench_nodes.push_back({i, ExprKind::BVAR, i, "", {}});
    }
    for (uint32_t i = 16; i < 31; ++i) {
        bench_nodes.push_back({i, ExprKind::APP, 0, "", {(i - 16) * 2, (i - 16) * 2 + 1}});
    }
    constexpr size_t ITERS = 20000;
    double checksum = 0.0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        Quat q = op.fold_dag(bench_nodes, 30, nullptr);
        checksum += q.w;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == -1e9) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
