#pragma once

#include <cstdint>
#include <optional>

struct ZPhi {
    int64_t a = 0;
    int64_t b = 0;

    constexpr bool operator==(const ZPhi& o) const = default;

    constexpr ZPhi operator+(const ZPhi& o) const { return {a + o.a, b + o.b}; }
    constexpr ZPhi operator-(const ZPhi& o) const { return {a - o.a, b - o.b}; }
    constexpr ZPhi operator-() const { return {-a, -b}; }
    constexpr ZPhi operator*(const ZPhi& o) const {
        return {a * o.a + b * o.b, a * o.b + b * o.a + b * o.b};
    }

    constexpr ZPhi conj() const {
        // TODO: Return Galois conjugate (a + b, -b)
        return {};
    }

    constexpr int64_t norm() const {
        // TODO: Return algebraic norm a^2 + a*b - b^2
        return 0;
    }

    constexpr std::optional<ZPhi> inv() const {
        // TODO: Return multiplicative inverse in Z[phi] iff norm() == 1 or -1
        return std::nullopt;
    }
};
