#pragma once

#include <cstddef>
#include <cstdint>

struct BipolarByteLUT {
    alignas(32) int16_t table[256][8]{};

    constexpr BipolarByteLUT() {
        // TODO: populate table[b][k] = ((b >> k) & 1) ? 1 : -1 for b in 0..255, k in 0..7
    }
};

static constexpr BipolarByteLUT BYTE_BIPOLAR_LUT{};

// Expand a 64-bit word into 64 signed int16_t values in {-1, +1} using BYTE_BIPOLAR_LUT
inline void expand_word_bipolar(uint64_t word, int16_t* out_64) {
    // TODO: expand 8 bytes of `word` into out_64[0..63]
    (void)word;
    (void)out_64;
}
