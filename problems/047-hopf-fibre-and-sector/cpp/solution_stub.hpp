#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

struct Vec3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

struct Quat {
    double w{1.0};
    double x{0.0};
    double y{0.0};
    double z{0.0};

    Quat() = default;
    Quat(double w_, double x_, double y_, double z_) : w(w_), x(x_), y(y_), z(z_) {}

    Quat normalize() const {
        double n = std::sqrt(w * w + x * x + y * y + z * z);
        if (n < 1e-12) return Quat(1.0, 0.0, 0.0, 0.0);
        return Quat(w / n, x / n, y / n, z / n);
    }
};

inline Vec3 hopf_project(const Quat& q) {
    return Vec3{
        2.0 * (q.x * q.z + q.w * q.y),
        2.0 * (q.y * q.z - q.w * q.x),
        q.w * q.w + q.z * q.z - q.x * q.x - q.y * q.y
    };
}

// Extract kept S^1 fibre phase angle psi in [-pi, pi]
inline double hopf_fibre_angle(const Quat& q) {
    // TODO: return atan2(q.z, q.w) if w^2 + z^2 > 1e-24, else atan2(q.y, q.x)
    (void)q;
    return 0.0;
}

// Reconstruct unit quaternion q in S^3 from S^2 base point and S^1 fibre angle psi
inline Quat reconstruct_from_hopf(const Vec3& s2, double psi) {
    // TODO: invert Hopf projection using kept fibre angle psi
    (void)s2; (void)psi;
    return Quat{};
}

// Quantize S^2 point (X, Y, Z) in [-1, 1]^3 into an 8x8x8 = 512 sector index [0, 511]
inline uint16_t quantize_hopf_sector_512(const Vec3& s2) {
    // TODO: map each axis [-1, 1] -> [0, 7] and pack (bx << 6) | (by << 3) | bz
    (void)s2;
    return 0;
}
