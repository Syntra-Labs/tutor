#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string_view>

struct ByteMarkovModel {
    std::array<std::array<uint32_t, 256>, 256> bigram{};
    std::array<uint32_t, 256> unigram{};
    uint64_t total_bytes = 0;

    void observe_stream(std::string_view text) {
        // TODO: Sample text with stride step = (text.size() > 4096) ? (text.size() / 4096) : 1
        // and update unigram, bigram, and total_bytes
        (void)text;
    }

    inline double byte_surprisal(uint8_t prev, uint8_t cur) const {
        // TODO: Compute interpolated Dirichlet-smoothed Order-2 byte surprisal in bits:
        // P(cur | prev) = 0.76 * (bigram[prev][cur] + 0.4) / (unigram[prev] + 102.4)
        //               + 0.24 * (unigram[cur] + 0.4) / (total_bytes + 102.4)
        // return -log2(max(P, 1e-12))
        (void)prev;
        (void)cur;
        return 0.0;
    }
};
