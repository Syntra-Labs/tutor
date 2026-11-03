#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

inline double freq_72edo(uint32_t char_code) {
    // TODO: 261.63 * 2^((char_code % 72) / 72.0)
    (void)char_code;
    return 0.0;
}

inline double note_duration_sec(uint32_t char_code, double master_speed = 1.0) {
    // TODO: ((45 + ((char_code * 5) % 6) * 20) / 1000.0) / master_speed
    (void)char_code;
    (void)master_speed;
    return 0.0;
}

constexpr inline uint32_t pitch_class_distance_72edo(uint32_t c1, uint32_t c2) {
    // TODO: Circular distance on Z_72 in [0, 36]
    (void)c1;
    (void)c2;
    return 0;
}
