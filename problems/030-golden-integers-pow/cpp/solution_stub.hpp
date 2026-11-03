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
    constexpr ZPhi conj() const { return {a + b, -b}; }
    constexpr int64_t norm() const { return a * a + a * b - b * b; }
    constexpr std::optional<ZPhi> inv() const {
        int64_t n = norm();
        if (n == 1) return conj();
        if (n == -1) return -conj();
        return std::nullopt;
    }

    constexpr ZPhi pow(int64_t n) const {
        // TODO: Binary exponentiation supporting n >= 0 and n < 0 (via *inv() when n < 0)
        (void)n;
        return {};
    }
};

constexpr inline ZPhi golden_pow(int64_t n) {
    return ZPhi{0, 1}.pow(n);
}
