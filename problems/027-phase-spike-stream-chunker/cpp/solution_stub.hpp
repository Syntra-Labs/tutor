#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>

struct ByteMarkovModel {
    std::array<std::array<uint32_t, 256>, 256> bigram{};
    std::array<uint32_t, 256> unigram{};
    uint64_t total_bytes = 0;

    void observe_stream(std::string_view text) {
        if (text.empty()) return;
        size_t step = (text.size() > 4096) ? (text.size() / 4096) : 1;
        uint8_t prev = static_cast<uint8_t>(text[0]);
        unigram[prev]++;
        total_bytes++;
        for (size_t i = step; i < text.size(); i += step) {
            uint8_t cur = static_cast<uint8_t>(text[i]);
            bigram[prev][cur]++;
            unigram[cur]++;
            total_bytes++;
            prev = cur;
        }
    }

    inline double byte_surprisal(uint8_t prev, uint8_t cur) const {
        double d = static_cast<double>(total_bytes);
        double v = (static_cast<double>(unigram[cur]) + 0.4) / (d + 102.4);
        double w = static_cast<double>(unigram[prev]);
        double y = static_cast<double>(bigram[prev][cur]);
        double p = 0.76 * ((y + 0.4) / (w + 102.4)) + 0.24 * v;
        return -std::log2(std::max(p, 1e-12));
    }
};

struct StreamChunk {
    uint32_t start_byte;
    uint32_t end_byte;
    uint32_t chunk_idx;
};

inline uint32_t count_demarcs(uint64_t seed_bits) {
    // TODO: Compute Rule 184 forward/counterfactual and Rule 30 1-step evolution,
    // then return popcount(matches ^ rotl(matches, 1))
    (void)seed_bits;
    return 0;
}

inline std::vector<StreamChunk> chunk_unformatted_byte_stream(
    std::string_view content,
    const ByteMarkovModel* markov = nullptr
) {
    // TODO: Segment content into contiguous StreamChunks using EMA surprisal + CA demarcs
    (void)content;
    (void)markov;
    return {};
}
