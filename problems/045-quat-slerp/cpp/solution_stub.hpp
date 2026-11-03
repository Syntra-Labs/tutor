#pragma once

#include <algorithm>
#include <cmath>

struct Quat {
    double w{1.0};
    double x{0.0};
    double y{0.0};
    double z{0.0};

    Quat() = default;
    Quat(double w_, double x_, double y_, double z_) : w(w_), x(x_), y(y_), z(z_) {}

    static Quat identity() { return Quat(1.0, 0.0, 0.0, 0.0); }

    double norm_sq() const { return w * w + x * x + y * y + z * z; }
    double norm() const { return std::sqrt(norm_sq()); }

    Quat normalize() const {
        double n = norm();
        if (n < 1e-12) return identity();
        return Quat(w / n, x / n, y / n, z / n);
    }

    double dot(const Quat& q) const {
        return w * q.w + x * q.x + y * q.y + z * q.z;
    }

    double geodesic_dist(const Quat& q) const {
        double d = std::min(1.0, std::abs(dot(q)));
        return 2.0 * std::acos(d);
    }
};

// Shortest-path SLERP on S^3 with small-angle NLERP fallback when |q1 . q2| > 0.9995
inline Quat slerp(Quat q1, Quat q2, double t) {
    // TODO: implement shortest-path SLERP with antipodal flip and >0.9995 NLERP fallback
    (void)q1; (void)q2; (void)t;
    return Quat::identity();
}
