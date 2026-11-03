#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

struct SurprisalSketch {
    static constexpr size_t SURPRISAL_BUCKETS = 65536;
    std::array<uint32_t, SURPRISAL_BUCKETS> bucket_counts{};
    uint32_t total_spans = 1;

    void clear() {
        // TODO: Reset bucket_counts to 0 and total_spans to 1
    }

    void observe_seed(uint64_t seed, int32_t delta = 1) {
        // TODO: Update bucket_counts[(seed ^ (seed >> 17)) & 65535] with saturation at 0
        (void)seed;
        (void)delta;
    }

    void step_span(int32_t delta = 1) {
        // TODO: Update total_spans (keeping total_spans >= 1 on negative delta)
        (void)delta;
    }

    int16_t weight_for_seed(uint64_t seed, int16_t base_weight = 4) const {
        // TODO: Compute stepped IDF salience weight in [1, base_weight + 2]
        (void)seed;
        return base_weight;
    }
};
