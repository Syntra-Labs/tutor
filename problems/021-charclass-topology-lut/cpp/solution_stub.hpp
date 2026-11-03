#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

enum class CharClass : uint8_t {
    SPACE = 0,
    LOWER = 1,
    UPPER = 2,
    DIGIT = 3,
    OP_MATH = 4,
    DELIM = 5,
    OTHER = 6
};

struct CharClassLUT {
    std::array<CharClass, 256> table{};

    constexpr CharClassLUT() {
        // TODO: Initialize 256-entry character-class table:
        //   Default: CharClass::OTHER
        //   SPACE:   ' ', '\t', '\r', '\n'
        //   LOWER:   'a'..'z'
        //   UPPER:   'A'..'Z'
        //   DIGIT:   '0'..'9'
        //   OP_MATH: "+-*/=<>!&|^~%\\"
        //   DELIM:   "(){}[].,;:_#@$\"'`"
        for (int i = 0; i < 256; ++i) {
            table[i] = CharClass::OTHER;
        }
    }
};

inline constexpr CharClassLUT CHAR_CLASS_LUT{};

constexpr inline CharClass classify_byte(uint8_t c) {
    // TODO: Lookup byte in CHAR_CLASS_LUT
    (void)c;
    return CharClass::OTHER;
}

struct TopologyFluxStats {
    double transition_flux = 0.0;
    double op_density      = 0.0;
    double delim_density   = 0.0;
};

inline TopologyFluxStats compute_topology_flux(std::string_view text) {
    // TODO: Compute transition_flux, op_density, and delim_density
    // using step = (text.size() > 1024) ? (text.size() / 1024) : 1
    (void)text;
    return {};
}
