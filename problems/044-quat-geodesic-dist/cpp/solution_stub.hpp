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

    Quat conj() const { return Quat(w, -x, -y, -z); }

    Quat mul(const Quat& q) const {
        return Quat(
            w * q.w - x * q.x - y * q.y - z * q.z,
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w
        );
    }

    // 4D Euclidean inner product
    double dot(const Quat& q) const {
        // TODO: return w*q.w + x*q.x + y*q.y + z*q.z
        (void)q;
        return 0.0;
    }

    // Geodesic distance on S^3 / {+1, -1} in [0, pi]
    double geodesic_dist(const Quat& q) const {
        // TODO: return 2 * acos(min(1.0, |dot(q)|))
        (void)q;
        return 0.0;
    }
};
