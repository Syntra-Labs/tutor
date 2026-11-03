#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <set>

// Element a + b * phi in Z[phi], where phi^2 = phi + 1
struct ZPhi {
    int32_t a{0};
    int32_t b{0};

    auto operator<=>(const ZPhi&) const = default;

    double to_double() const {
        return static_cast<double>(a) + static_cast<double>(b) * 1.6180339887498948482;
    }
};

inline ZPhi zphi_add(ZPhi x, ZPhi y) {
    return {x.a + y.a, x.b + y.b};
}

inline ZPhi zphi_mul(ZPhi x, ZPhi y) {
    return {
        x.a * y.a + x.b * y.b,
        x.a * y.b + x.b * y.a + x.b * y.b
    };
}

// Doubled icosian 2*q in Z[phi]^4
using Icosian2 = std::array<ZPhi, 4>;

// Generate all 120 doubled unit icosians in sorted order
inline std::array<Icosian2, 120> generate_icosians_120() {
    // TODO: generate 8 axis + 16 hypercube + 96 even-permutation golden vertices in Z[phi]^4
    return {};
}

// Compute 4 * (p . q) = sum_{k=0..3} P_k * Q_k in Z[phi]
inline ZPhi icosian_dot4(const Icosian2& p, const Icosian2& q) {
    // TODO: sum zphi_mul(p[k], q[k]) for k = 0..3
    (void)p; (void)q;
    return {};
}

// Partition the 120 icosians by inner product with icosians[ref_idx] from largest to smallest
// real value into 9 shells and return their counts
inline std::array<uint32_t, 9> compute_icosian_shell_counts(
    const std::array<Icosian2, 120>& icosians,
    size_t ref_idx = 0
) {
    // TODO: group icosian_dot4(icosians[ref_idx], q) into 9 shells sorted by descending value
    (void)icosians; (void)ref_idx;
    return {};
}
