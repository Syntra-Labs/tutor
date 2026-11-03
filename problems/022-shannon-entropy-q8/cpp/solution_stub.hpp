#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string_view>

inline double compute_shannon_entropy(std::string_view text) {
    // TODO: Accumulate 256-bin byte frequency histogram (using stride step = text.size() / 1024 if > 1024)
    // and return H = -sum_{p > 0} p * log2(p)
    (void)text;
    return 0.0;
}

inline uint8_t quantize_entropy_q8(double entropy_bits) {
    // TODO: Quantize H in [0, 8] to uint8_t via clamp(static_cast<int>(entropy_bits * 32.0), 0, 255)
    (void)entropy_bits;
    return 0;
}

inline uint8_t compute_entropy_q8(std::string_view text) {
    return quantize_entropy_q8(compute_shannon_entropy(text));
}
