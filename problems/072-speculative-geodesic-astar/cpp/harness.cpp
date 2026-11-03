#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 072 Speculative Geodesic A* Search Test Suite ===\n";

    SpeculativeGeodesicRouter router(0.05, 2);
    std::unordered_map<std::string, std::vector<double>> actions = {
        {"intro", {1.0, 0.0, 0.0, 0.0}},
        {"rw_add", {0.98, 0.198, 0.0, 0.0}}, // cos with (1,0,0,0) is ~0.9802 (margin < 0.05!)
        {"rfl", {0.0, 1.0, 0.0, 0.0}},
        {"omega", {0.0, 0.0, 1.0, 0.0}}
    };

    // Test 1: Cosine ranking and speculative queue margin gating
    {
        // Close margin between "intro" (1.0) and "rw_add" (~0.9802) -> triggers speculation (2 items)
        auto ranked = router.rank_candidates({1.0, 0.0, 0.0, 0.0}, actions);
        assert(ranked.size() == 4);
        assert(ranked[0].name == "intro");
        assert(ranked[1].name == "rw_add");

        auto [q_spec, margin_spec, is_spec] = router.select_speculative_queue(ranked);
        assert(is_spec == true);
        assert(q_spec.size() == 2);

        // Wide margin along (0, 1, 0, 0) -> "rfl" has score 1.0, next is ~0.198 -> non-speculative (1 item)
        auto ranked_wide = router.rank_candidates({0.0, 1.0, 0.0, 0.0}, actions);
        auto [q_wide, margin_wide, is_wide] = router.select_speculative_queue(ranked_wide);
        assert(is_wide == false);
        assert(q_wide.size() == 1);
        assert(q_wide[0].name == "rfl");
        std::cout << "  • Test 1: Tangent cosine ranking and margin-gated speculation ... [PASS]\n";
    }

    // Test 2: Speculative fallback when rank-1 fails and rank-2 succeeds
    {
        SpeculativeGeodesicProver prover(router, 20, 5);
        auto policy = [](const std::string& g) -> std::vector<double> {
            if (g == "G0") return {1.0, 0.0, 0.0, 0.0}; // queues {"intro", "rw_add"}
            return {0.0, 1.0, 0.0, 0.0};                // queues {"rfl"}
        };
        auto kernel = [](const std::string& g, const std::string& tac)
            -> std::pair<KernelStatus, std::vector<std::string>> {
            if (g == "G0" && tac == "intro") return {KernelStatus::KernelError, {}}; // #0 fails!
            if (g == "G0" && tac == "rw_add") return {KernelStatus::SubgoalsOpen, {"G1"}}; // #1 succeeds!
            if (g == "G1" && tac == "rfl") return {KernelStatus::GoalsClosed, {}};
            return {KernelStatus::KernelError, {}};
        };

        auto res = prover.search("G0", policy, actions, kernel);
        assert(res.status == SearchStatus::SuccessProven);
        assert((res.proof_steps == std::vector<std::string>{"rw_add", "rfl"}));
        assert(res.kernel_calls == 3);
        std::cout << "  • Test 2: Speculative fallback recovery & proof path ... [PASS]\n";
    }

    // Test 3: First speculative candidate succeeds -> increments repl_calls_saved
    {
        SpeculativeGeodesicProver prover(router, 20, 5);
        auto policy = [](const std::string&) -> std::vector<double> {
            return {1.0, 0.0, 0.0, 0.0}; // queues {"intro", "rw_add"}
        };
        auto kernel = [](const std::string& g, const std::string& tac)
            -> std::pair<KernelStatus, std::vector<std::string>> {
            if (g == "G0" && tac == "intro") return {KernelStatus::GoalsClosed, {}};
            return {KernelStatus::KernelError, {}};
        };

        auto res = prover.search("G0", policy, actions, kernel);
        assert(res.status == SearchStatus::SuccessProven);
        assert(res.repl_calls_saved == 1);
        assert(res.kernel_calls == 1);
        std::cout << "  • Test 3: Speculative hit repl_calls_saved counter ... [PASS]\n";
    }

    // Performance benchmark
    SpeculativeGeodesicProver bench_prover(router, 20, 5);
    auto b_policy = [](const std::string& g) -> std::vector<double> {
        return (g == "G0") ? std::vector<double>{1.0, 0.0, 0.0, 0.0} : std::vector<double>{0.0, 1.0, 0.0, 0.0};
    };
    auto b_kernel = [](const std::string& g, const std::string& tac)
        -> std::pair<KernelStatus, std::vector<std::string>> {
        if (g == "G0" && tac == "intro") return {KernelStatus::SubgoalsOpen, {"G1"}};
        if (g == "G1" && tac == "rfl") return {KernelStatus::GoalsClosed, {}};
        return {KernelStatus::KernelError, {}};
    };

    constexpr size_t ITERS = 20000;
    size_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto r = bench_prover.search("G0", b_policy, actions, b_kernel);
        checksum += r.proof_steps.size();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
