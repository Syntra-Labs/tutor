#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <vector>

struct Quat {
    double w{1.0};
    double x{0.0};
    double y{0.0};
    double z{0.0};

    Quat() = default;
    Quat(double w_, double x_, double y_, double z_) : w(w_), x(x_), y(y_), z(z_) {}

    static Quat identity() { return Quat(1.0, 0.0, 0.0, 0.0); }

    Quat normalize() const {
        double n = std::sqrt(w * w + x * x + y * y + z * z);
        return (n < 1e-12) ? identity() : Quat(w / n, x / n, y / n, z / n);
    }

    double geodesic_dist(const Quat& q) const {
        Quat p = normalize();
        Quat r = q.normalize();
        double d = std::abs(p.w * r.w + p.x * r.x + p.y * r.y + p.z * r.z);
        if (d > 1.0) d = 1.0;
        return 2.0 * std::acos(d);
    }
};

enum class MacroActionKind {
    Tier2A_DirectTerminating,
    Tier2B_ForwardInvention
};

struct GoalState {
    Quat z{Quat::identity()};
    std::string conclusion{};
};

struct MacroRouteDecision {
    MacroActionKind kind{MacroActionKind::Tier2B_ForwardInvention};
    double geodesic_distance{0.0};
    bool matched_terminal_head{false};
    bool fallback_triggered{false};
    std::vector<std::string> candidate_tactics{};
    std::string reason{};
};

class MacroActionDecider {
public:
    explicit MacroActionDecider(double theta_conv = 0.5, Quat z_qed = Quat::identity())
        : theta_conv_(theta_conv), z_qed_(z_qed) {}

    static const std::vector<std::string>& get_tier2a_tactics() {
        static const std::vector<std::string> t2a = {"exact", "rfl", "simp", "omega", "ring", "linarith", "decide"};
        return t2a;
    }

    static const std::vector<std::string>& get_tier2b_tactics() {
        static const std::vector<std::string> t2b = {"apply", "rw", "intro", "cases", "induction"};
        return t2b;
    }

    static bool is_reflexive_or_terminal_head(const std::string& conclusion) {
        // TODO: detect True / rfl / lhs == rhs after optional turnstile `⊢`
        (void)conclusion;
        return false;
    }

    MacroRouteDecision route(const GoalState& state) const {
        // TODO: route to Tier 2A if reflexive/terminal head or d_S3(z, z_qed) <= theta_conv_, else Tier 2B
        (void)state;
        return {};
    }

    MacroRouteDecision route_with_feedback(
        const GoalState& state,
        const std::function<bool(const std::string&)>& kernel_eval
    ) const {
        // TODO: if Tier 2A is selected but all Tier 2A tactics fail kernel_eval, fall back to Tier 2B
        (void)state;
        (void)kernel_eval;
        return {};
    }

private:
    double theta_conv_{0.5};
    Quat z_qed_{Quat::identity()};
};
