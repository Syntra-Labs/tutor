#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

using Mat8 = std::array<std::array<int8_t, 8>, 8>;

inline constexpr std::array<std::array<uint8_t, 3>, 7> FANO_CYCLES = {{
    {1, 2, 4}, {2, 3, 5}, {3, 4, 6}, {4, 5, 7},
    {5, 6, 1}, {6, 7, 2}, {7, 1, 3}
}};

inline std::pair<int8_t, uint8_t> octonion_basis_product(uint8_t left, uint8_t right) {
    if (left == 0) return {1, right};
    if (right == 0) return {1, left};
    if (left == right) return {-1, 0};
    for (const auto& cyc : FANO_CYCLES) {
        uint8_t a = cyc[0], b = cyc[1], c = cyc[2];
        if (left == a && right == b) return {1, c};
        if (left == b && right == c) return {1, a};
        if (left == c && right == a) return {1, b};
        if (left == b && right == a) return {-1, c};
        if (left == c && right == b) return {-1, a};
        if (left == a && right == c) return {-1, b};
    }
    return {0, 0};
}

// Return 8x8 identity matrix I_8
inline Mat8 mat_identity() {
    // TODO: return 8x8 identity matrix
    return Mat8{};
}

// Multiply two 8x8 integer matrices A * B
inline Mat8 mat_mul(const Mat8& A, const Mat8& B) {
    // TODO: implement 8x8 matrix multiplication
    (void)A; (void)B;
    return Mat8{};
}

// Construct the 6 left-multiplication generator matrices G_0..G_5 (for e_1..e_6)
inline std::array<Mat8, 6> build_cl06_generators() {
    // TODO: populate G[i][idx][col] = sgn from octonion_basis_product(i + 1, col)
    return {};
}

// Construct the 15 bivector matrices B_ij = G_i * G_j for 0 <= i < j < 6 in lexicographical order
inline std::array<Mat8, 15> build_cl06_bivectors() {
    // TODO: compute pairwise products G_i * G_j for 0 <= i < j < 6
    return {};
}
