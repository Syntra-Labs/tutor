#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

struct LucidityEval {
    double bmu_sim = 0.5;
    double archetype_sim = 0.5;
    double q_error = 1.5;
    double lucidity = 0.0;
};

struct DeltaFeedback {
    bool updated = false;
    double surprise = 0.0;
    int16_t delta_w = 0;
};

inline LucidityEval compute_lucidity_eval(double bmu_sim, double archetype_sim) {
    // TODO: implement BMU quantization error and manifold lucidity readout
    (void)bmu_sim;
    (void)archetype_sim;
    return {};
}

inline DeltaFeedback compute_surprise_delta(double query_target_sim, bool positive_reward = true) {
    // TODO: implement surprise-gated delta learning rule
    (void)query_target_sim;
    (void)positive_reward;
    return {};
}
