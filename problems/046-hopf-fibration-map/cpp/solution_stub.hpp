#pragma once

#include <cmath>

struct Vec3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};

    double norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }
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

    Quat mul(const Quat& q) const {
        return Quat(
            w * q.w - x * q.x - y * q.y - z * q.z,
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w
        );
    }
};

// Construct U(1) fibre rotor e_psi = (cos(psi), 0, 0, sin(psi))
inline Quat hopf_phase_rotor(double psi) {
    // TODO: return (cos(psi), 0, 0, sin(psi))
    (void)psi;
    return Quat{};
}

// Project unit quaternion q=(w,x,y,z) onto S^2 via (2(xz+wy), 2(yz-wx), w^2+z^2-x^2-y^2)
inline Vec3 hopf_project(const Quat& q) {
    // TODO: implement Hopf projection S^3 -> S^2
    (void)q;
    return Vec3{};
}
