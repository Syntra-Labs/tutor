#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

enum class KernelStatus {
    GoalsClosed,
    SubgoalsOpen,
    KernelError,
    KernelTimeout
};

enum class SearchStatus {
    Searching,
    SuccessProven,
    BudgetExhausted,
    SearchFailed
};

struct TacticCandidate {
    std::string name{};
    std::vector<double> vector{};
    double score{0.0};

    TacticCandidate() = default;
    TacticCandidate(std::string n, std::vector<double> v, double s = 0.0)
        : name(std::move(n)), vector(std::move(v)), score(s) {}
};

struct SearchNode {
    std::string state_id{};
    std::string goal_str{};
    int depth{0};
    std::shared_ptr<SearchNode> parent{nullptr};
    std::string applied_tactic{};

    SearchNode() = default;
    SearchNode(std::string id, std::string goal, int d = 0,
               std::shared_ptr<SearchNode> p = nullptr, std::string tac = "")
        : state_id(std::move(id)), goal_str(std::move(goal)), depth(d),
          parent(std::move(p)), applied_tactic(std::move(tac)) {}
};

struct GeodesicSearchResult {
    SearchStatus status{SearchStatus::Searching};
    std::vector<std::string> proof_steps{};
    int total_steps{0};
    int kernel_calls{0};
    int repl_calls_saved{0};
    int depth_reached{0};
};

class SpeculativeGeodesicRouter {
public:
    explicit SpeculativeGeodesicRouter(double margin_threshold = 0.05, int max_beam_width = 2)
        : margin_threshold_(margin_threshold), max_beam_width_(max_beam_width) {}

    std::vector<TacticCandidate> rank_candidates(
        const std::vector<double>& tangent_vector,
        const std::unordered_map<std::string, std::vector<double>>& action_dict
    ) const {
        // TODO: rank tactics by cosine similarity to tangent_vector
        (void)tangent_vector;
        (void)action_dict;
        return {};
    }

    std::tuple<std::vector<TacticCandidate>, double, bool> select_speculative_queue(
        const std::vector<TacticCandidate>& ranked
    ) const {
        // TODO: select 1 or 2 speculative candidates based on margin_threshold_
        (void)ranked;
        return std::make_tuple(std::vector<TacticCandidate>{}, 0.0, false);
    }

private:
    double margin_threshold_{0.05};
    int max_beam_width_{2};
};

class SpeculativeGeodesicProver {
public:
    using PolicyFn = std::function<std::vector<double>(const std::string&)>;
    using KernelEvalFn = std::function<std::pair<KernelStatus, std::vector<std::string>>(
        const std::string&, const std::string&)>;

    explicit SpeculativeGeodesicProver(
        SpeculativeGeodesicRouter router = SpeculativeGeodesicRouter(),
        int max_steps = 50,
        int max_depth = 10
    ) : router_(router), max_steps_(max_steps), max_depth_(max_depth) {}

    GeodesicSearchResult search(
        const std::string& initial_goal,
        const PolicyFn& policy_fn,
        const std::unordered_map<std::string, std::vector<double>>& action_dict,
        const KernelEvalFn& kernel_eval_fn
    ) {
        // TODO: implement speculative geodesic search over proof states
        (void)initial_goal;
        (void)policy_fn;
        (void)action_dict;
        (void)kernel_eval_fn;
        return {};
    }

private:
    SpeculativeGeodesicRouter router_;
    int max_steps_{50};
    int max_depth_{10};
};
