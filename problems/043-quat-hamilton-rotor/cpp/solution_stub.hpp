#pragma once

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

    // Hamilton product: (*this) * q
    Quat mul(const Quat& q) const {
        // TODO: implement non-commutative Hamilton product
        (void)q;
        return identity();
    }

    // Sandwich transformation: r * (*this) * r.conj()
    Quat rotate(const Quat& r) const {
        // TODO: implement sandwich rotation r * (*this) * conj(r)
        (void)r;
        return identity();
    }
};

// Build unit rotor on S^3 from axis (ux, uy, uz) and angle_rad
inline Quat make_rotor(double angle_rad, double ux, double uy, double uz) {
    // TODO: implement half-angle rotor construction with 1e-12 zero-axis guard
    (void)angle_rad; (void)ux; (void)uy; (void)uz;
    return Quat::identity();
}
