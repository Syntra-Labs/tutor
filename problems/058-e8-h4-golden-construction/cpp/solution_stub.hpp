#pragma once

#include <algorithm>
#include <array>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <set>

struct ZPhi {
    int32_t a{0};
    int32_t b{0};

    auto operator<=>(const ZPhi&) const = default;
};

inline ZPhi zphi_add(ZPhi x, ZPhi y) { return {x.a + y.a, x.b + y.b}; }
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

// Construct the 240 roots 2I U phi*2I in doubled Z[phi]^4 coordinates
inline std::array<Icosian2, 240> build_e8_golden_roots_240(
    const std::array<Icosian2, 120>& icosians
) {
    // TODO: copy 120 icosians into [0..119] and phi * icosians into [120..239]
    (void)icosians;
    return {};
}

// Evaluate reduced bilinear form <x, y> = 2 * rational_part(x . y) = m / 2 where 4*(x.y) = m + n*phi
inline int32_t e8_reduced_inner_product(const Icosian2& A, const Icosian2& B) {
    // TODO: compute sum_{k=0..3} zphi_mul(A[k], B[k]).a / 2
    (void)A; (void)B;
    return 0;
}

// Count inner products of roots[ref_idx] with all 240 roots at values {+2, +1, 0, -1, -2}
inline std::array<uint32_t, 5> e8_inner_product_distribution(
    const std::array<Icosian2, 240>& roots,
    size_t ref_idx
) {
    // TODO: return counts for inner product == {+2, +1, 0, -1, -2}
    (void)roots; (void)ref_idx;
    return {};
}

// Compute Weyl reflection x - <x, r> * r in doubled Z[phi]^4 coordinates
inline Icosian2 e8_weyl_reflect(const Icosian2& x, const Icosian2& r) {
    // TODO: compute x[k] - ip * r[k] for k=0..3 where ip = e8_reduced_inner_product(x, r)
    (void)x; (void)r;
    return {};
}
