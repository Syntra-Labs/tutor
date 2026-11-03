#include <cassert>
#include <chrono>
#include <iostream>
#include <string>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 068 Reversible Alpha & Premise Canonicalizer Test Suite ===\n";

    // Test 1: Declaration to turnstile conversion
    {
        std::string decl = "theorem add_comm_eq (a b : Nat) (hab : a = b) : b + a = a + a := by";
        std::string ts = to_turnstile_state(decl);
        assert(ts == "a b : Nat\nhab : a = b\n⊢ b + a = a + a");
        std::cout << "  • Test 1: Theorem declaration to turnstile state ... [PASS]\n";
    }

    // Test 2: Alpha-invariance across distinct variable & hypothesis names
    {
        std::string g1 = "theorem t1 (a b : Nat) (hab : a = b) : b = a := by";
        std::string g2 = "theorem t2 (x y : Nat) (h_eq : x = y) : y = x := by";
        auto c1 = canonicalize_goal_and_tactic(g1);
        auto c2 = canonicalize_goal_and_tactic(g2);
        assert(!c1.canon_goal.empty());
        assert(c1.canon_goal == c2.canon_goal);
        assert(c1.canon_goal == "v0 v1 : Nat\nh0 : v0 = v1\n⊢ v1 = v0");
        std::cout << "  • Test 2: Alpha-equivalence produces identical IR ... [PASS]\n";
    }

    // Test 3: Premise canonicalization (p0, p1) and lossless round-trip invert_sigma
    {
        std::string goal = "m n : Nat\nh_le : m = n\n⊢ m + 0 = n";
        std::string tactic = "rw [Nat.add_zero, h_le]";
        auto c = canonicalize_goal_and_tactic(goal, tactic);
        assert(c.canon_tactic == "rw [p0, h0]");
        std::string restored = invert_sigma(c.canon_tactic, c.sigma);
        assert(restored == tactic);
        std::cout << "  • Test 3: External lemma premise IDs and invert_sigma round-trip ... [PASS]\n";
    }

    // Performance benchmark
    const std::string b_goal = "theorem foo (x y z : Nat) (h1 : x = y) (h2 : y = z) : x + z = y + y := by";
    const std::string b_tac  = "rw [h1, h2, Nat.add_comm]";
    constexpr size_t ITERS = 20000;
    size_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto c = canonicalize_goal_and_tactic(b_goal, b_tac);
        auto r = invert_sigma(c.canon_tactic, c.sigma);
        checksum += r.size();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
