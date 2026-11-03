#pragma once

#include <cmath>

struct Quat {
    double w{1.0};
    double x{0.0};
    double y{0.0};
    double z{0.0};

    Quat() = default;
    Quat(double w_, double x_, double y_, double z_) : w(w_), x(x_), y(y_), z(z_) {}

    static Quat identity() {
        return Quat(1.0, 0.0, 0.0, 0.0);
    }

    double norm_sq() const {
        // TODO: return w^2 + x^2 + y^2 + z^2
        return 0.0;
    }

    double norm() const {
        // TODO: return Euclidean norm
        return 0.0;
    }

    Quat normalize() const {
        // TODO: project onto S^3; return identity() if norm() < 1e-12
        return identity();
    }

    Quat conj() const {
        // TODO: return quaternion conjugate (w, -x, -y, -z)
        return identity();
    }

    Quat inverse() const {
        // TODO: return multiplicative inverse conj() / norm_sq(); return identity() if norm_sq() < 1e-24
        return identity();
    }
};
