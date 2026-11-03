#pragma once

#include <cstdint>

struct ZPhi {
    int64_t a = 0;
    int64_t b = 0;

    constexpr bool operator==(const ZPhi& o) const = default;

    constexpr ZPhi operator+(const ZPhi& o) const {
        // TODO: (a + c, b + d)
        (void)o;
        return {};
    }

    constexpr ZPhi operator-(const ZPhi& o) const {
        // TODO: (a - c, b - d)
        (void)o;
        return {};
    }

    constexpr ZPhi operator-() const {
        // TODO: (-a, -b)
        return {};
    }

    constexpr ZPhi operator*(const ZPhi& o) const {
        // TODO: (a*c + b*d, a*d + b*c + b*d)
        (void)o;
        return {};
    }
};
