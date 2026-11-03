#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 071 Least-Action Macro-Action Decider Test Suite ===\n";

    MacroActionDecider decider(0.5, Quat(1.0, 0.0, 0.0, 0.0));

    // Test 1: Syntactic reflexivity detection on `⊢ lhs = rhs` and `⊢ True`
    {
        assert(MacroActionDecider::is_reflexive_or_terminal_head("v0 : Nat\n⊢ v0 + 1 = v0 + 1") == true);
        assert(MacroActionDecider::is_reflexive_or_terminal_head("⊢ True") == true);
        assert(MacroActionDecider::is_reflexive_or_terminal_head("⊢ v0 + 1 = v1 + 1") == false);
        assert(MacroActionDecider::is_reflexive_or_terminal_head("⊢ v0 != v0") == false);

        // Even with orthogonal quaternion (d_S3 = pi), reflexive head routes to Tier 2A
        GoalState st{Quat(0.0, 1.0, 0.0, 0.0), "⊢ x + y = x + y"};
        auto d = decider.route(st);
        assert(d.kind == MacroActionKind::Tier2A_DirectTerminating);
        assert(d.matched_terminal_head == true);
        assert(d.candidate_tactics == MacroActionDecider::get_tier2a_tactics());
        std::cout << "  • Test 1: Reflexive/terminal head routes to Tier 2A ... [PASS]\n";
    }

    // Test 2: Geodesic convergence ball vs divergent state
    {
        // Close to z_qed = (1, 0, 0, 0) -> d_S3 <= 0.5 -> Tier 2A
        GoalState near_st{Quat(0.99, 0.1, 0.0, 0.0), "⊢ a + b = b + a"};
        auto d_near = decider.route(near_st);
        assert(d_near.kind == MacroActionKind::Tier2A_DirectTerminating);
        assert(d_near.matched_terminal_head == false);

        // Far from z_qed -> d_S3 = pi > 0.5 -> Tier 2B
        GoalState far_st{Quat(0.0, 0.0, 1.0, 0.0), "⊢ a + b = b + a"};
        auto d_far = decider.route(far_st);
        assert(d_far.kind == MacroActionKind::Tier2B_ForwardInvention);
        assert(d_far.candidate_tactics == MacroActionDecider::get_tier2b_tactics());
        std::cout << "  • Test 2: S^3 geodesic convergence ball routing ... [PASS]\n";
    }

    // Test 3: Kernel rejection feedback triggers fallback from Tier 2A to Tier 2B
    {
        GoalState near_st{Quat(0.99, 0.05, 0.0, 0.0), "⊢ a + b = b + a"};
        auto d_fail = decider.route_with_feedback(near_st, [](const std::string&) { return false; });
        assert(d_fail.kind == MacroActionKind::Tier2B_ForwardInvention);
        assert(d_fail.fallback_triggered == true);

        auto d_pass = decider.route_with_feedback(near_st, [](const std::string& tac) { return tac == "omega"; });
        assert(d_pass.kind == MacroActionKind::Tier2A_DirectTerminating);
        assert(d_pass.fallback_triggered == false);
        std::cout << "  • Test 3: Kernel rejection fallback to Tier 2B ... [PASS]\n";
    }

    // Performance benchmark
    GoalState b_st{Quat(0.8, 0.6, 0.0, 0.0), "v0 v1 : Nat\n⊢ v0 + v1 = v1 + v0"};
    constexpr size_t ITERS = 100000;
    size_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        b_st.z.w = (i & 1) ? 0.99 : 0.2;
        auto d = decider.route(b_st);
        checksum += d.candidate_tactics.size();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
