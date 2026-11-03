#pragma once

#include <array>
#include <cstdint>
#include <utility>

inline constexpr std::array<std::array<uint8_t, 3>, 7> FANO_CYCLES = {{
    {1, 2, 4}, {2, 3, 5}, {3, 4, 6}, {4, 5, 7},
    {5, 6, 1}, {6, 7, 2}, {7, 1, 3}
}};

// Multiply basis elements e_left * e_right = sign * e_index for left, right in [0, 7]
inline std::pair<int8_t, uint8_t> octonion_basis_product(uint8_t left, uint8_t right) {
    // TODO: implement octonion basis multiplication using FANO_CYCLES
    (void)left; (void)right;
    return {0, 0};
}

// Count how many of the 210 ordered triples (i, j, k) of distinct imaginary units in 1..7
// satisfy (e_i * e_j) * e_k == - e_i * (e_j * e_k)
inline uint32_t count_anti_associative_triples() {
    // TODO: enumerate all 210 distinct imaginary triples and count anti-associative ones
    return 0;
}
