#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 069 Hash-Consed Expr DAG & Unification Gate Test Suite ===\n";

    HashConsDAG dag;
    int nat = dag.intern(ExprKind::CONST, "Nat");
    int add = dag.intern(ExprKind::CONST, "Nat.add");
    int zero = dag.intern(ExprKind::LIT, "0");
    int x = dag.intern(ExprKind::FVAR, "v0");
    int y = dag.intern(ExprKind::FVAR, "v1");

    // Test 1: Hash-consing deduplicates identical expressions to the same node ID
    {
        int x_plus_0_a = dag.mk_app(dag.mk_app(add, x), zero);
        int x_plus_0_b = dag.mk_app(dag.mk_app(add, x), zero);
        assert(x_plus_0_a == x_plus_0_b);
        std::cout << "  • Test 1: Hash-consing structural deduplication ... [PASS]\n";
    }

    // Test 2: First-order unification enforces consistent binder bindings
    {
        // Pattern: ∀ (a : Nat), a = a  --> FORALLE(Nat, Eq(Nat, BVAR(0), BVAR(0)))
        int bvar0 = dag.intern(ExprKind::BVAR, "0");
        int pat_concl = dag.mk_eq(nat, bvar0, bvar0);

        int eq_xx = dag.mk_eq(nat, x, x);
        int eq_xy = dag.mk_eq(nat, x, y);

        std::unordered_map<int, int> b1, b2;
        assert(match_expr(dag, pat_concl, eq_xx, 1, 0, b1) == true);
        assert(b1[0] == x);
        assert(match_expr(dag, pat_concl, eq_xy, 1, 0, b2) == false);
        std::cout << "  • Test 2: Consistent de Bruijn wildcard unification ... [PASS]\n";
    }

    // Test 3: Admissible move filtering (exact, rw, apply)
    {
        // Target: (x + 0) = y
        int x_plus_0 = dag.mk_app(dag.mk_app(add, x), zero);
        int target = dag.mk_eq(nat, x_plus_0, y);

        // Hyp h0 : x = y
        int h0_type = dag.mk_eq(nat, x, y);
        // Hyp h1 : (x + 0) = y (exact match!)
        int h1_type = dag.mk_eq(nat, x_plus_0, y);

        // Lemma Nat.add_zero : ∀ (n : Nat), n + 0 = n
        int bvar0 = dag.intern(ExprKind::BVAR, "0");
        int bvar_plus_0 = dag.mk_app(dag.mk_app(add, bvar0), zero);
        int add_zero_lemma = dag.intern(ExprKind::FORALLE, "n", {nat, dag.mk_eq(nat, bvar_plus_0, bvar0)});

        std::vector<Hyp> hyps = {{"h0", h0_type, true}, {"h1", h1_type, true}};
        std::vector<std::pair<std::string, int>> lemmas = {{"Nat.add_zero", add_zero_lemma}};

        auto moves = admissible_moves(dag, hyps, target, lemmas);

        auto has_move = [&](const Move& m) {
            return std::find(moves.begin(), moves.end(), m) != moves.end();
        };
        assert(has_move(Move{"exact", "h1", false}));
        assert(has_move(Move{"rw", "h0", false}));          // x occurs inside target (x + 0) = y
        assert(has_move(Move{"rw", "Nat.add_zero", false})); // n + 0 unifies with subterm x + 0
        std::cout << "  • Test 3: Admissible move gate (exact, rw, lemma unification) ... [PASS]\n";
    }

    // Performance benchmark
    int x_plus_0 = dag.mk_app(dag.mk_app(add, x), zero);
    int target = dag.mk_eq(nat, x_plus_0, y);
    int h0_type = dag.mk_eq(nat, x, y);
    int bvar0 = dag.intern(ExprKind::BVAR, "0");
    int bvar_plus_0 = dag.mk_app(dag.mk_app(add, bvar0), zero);
    int add_zero_lemma = dag.intern(ExprKind::FORALLE, "n", {nat, dag.mk_eq(nat, bvar_plus_0, bvar0)});
    std::vector<Hyp> hyps = {{"h0", h0_type, true}};
    std::vector<std::pair<std::string, int>> lemmas = {{"Nat.add_zero", add_zero_lemma}};

    constexpr size_t ITERS = 50000;
    size_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto m = admissible_moves(dag, hyps, target, lemmas);
        checksum += m.size();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
