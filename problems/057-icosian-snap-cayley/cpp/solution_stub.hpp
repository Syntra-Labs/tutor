#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <set>

struct ZPhi {
    int32_t a{0};
    int32_t b{0};

    auto operator<=>(const ZPhi&) const = default;

    double to_double() const {
        return static_cast<double>(a) + static_cast<double>(b) * 1.6180339887498948482;
    }
};

inline ZPhi zphi_add(ZPhi x, ZPhi y) { return {x.a + y.a, x.b + y.b}; }
inline ZPhi zphi_sub(ZPhi x, ZPhi y) { return {x.a - y.a, x.b - y.b}; }
inline ZPhi zphi_mul(ZPhi x, ZPhi y) {
    return {x.a * y.a + x.b * y.b, x.a * y.b + x.b * y.a + x.b * y.b};
}

using Icosian2 = std::array<ZPhi, 4>;

inline std::array<Icosian2, 120> generate_icosians_120() {
    std::set<Icosian2> s;
    for (int i = 0; i < 4; ++i) {
        for (int32_t v : {-2, 2}) {
            Icosian2 q{};
            q[i] = {v, 0};
            s.insert(q);
        }
    }
    for (int mask = 0; mask < 16; ++mask) {
        Icosian2 q{};
        for (int i = 0; i < 4; ++i) q[i] = {(mask & (1 << i)) ? 1 : -1, 0};
        s.insert(q);
    }
    std::array<int, 4> perm = {0, 1, 2, 3};
    do {
        int inv = 0;
        for (int a = 0; a < 4; ++a)
            for (int b = a + 1; b < 4; ++b)
                if (perm[a] > perm[b]) ++inv;
        if (inv % 2 == 0) {
            for (int m = 0; m < 8; ++m) {
                int32_t s0 = (m & 1) ? 1 : -1;
                int32_t s1 = (m & 2) ? 1 : -1;
                int32_t s2 = (m & 4) ? 1 : -1;
                std::array<ZPhi, 4> base = {{{0, s0}, {s1, 0}, {-s2, s2}, {0, 0}}};
                Icosian2 q{};
                for (int k = 0; k < 4; ++k) q[k] = base[perm[k]];
                s.insert(q);
            }
        }
    } while (std::next_permutation(perm.begin(), perm.end()));

    std::array<Icosian2, 120> out{};
    size_t idx = 0;
    for (const auto& q : s) out[idx++] = q;
    return out;
}

// Multiply two doubled icosians A = 2p, B = 2q and return 2*(p * q) in Z[phi]^4
inline Icosian2 icosian_mul(const Icosian2& A, const Icosian2& B) {
    // TODO: compute Hamilton product of A and B in Z[phi] and divide each component by 2
    (void)A; (void)B;
    return {};
}

// Build the 120x120 Cayley multiplication table for the binary icosahedral group 2I
inline std::array<std::array<uint8_t, 120>, 120> build_icosian_cayley_table(
    const std::array<Icosian2, 120>& icosians
) {
    // TODO: populate table[i][j] = index of icosian_mul(icosians[i], icosians[j])
    (void)icosians;
    return {};
}

// Snap a continuous quaternion (w, x, y, z) to the closest icosian index in [0, 119]
inline uint8_t snap_icosian(
    const std::array<Icosian2, 120>& icosians,
    double w, double x, double y, double z
) {
    // TODO: return index i in [0, 119] maximizing dot product with (w, x, y, z)
    (void)icosians; (void)w; (void)x; (void)y; (void)z;
    return 0;
}
