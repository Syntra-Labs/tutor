#pragma once

#include <cmath>

struct Quat {
    double w{0.0};
    double x{0.0};
    double y{0.0};
    double z{0.0};

    Quat() = default;
    Quat(double w_, double x_, double y_, double z_) : w(w_), x(x_), y(y_), z(z_) {}

    double norm_sq() const { return w * w + x * x + y * y + z * z; }
    double norm() const { return std::sqrt(norm_sq()); }
    Quat conj() const { return Quat(w, -x, -y, -z); }

    double dot(const Quat& q) const {
        return w * q.w + x * q.x + y * q.y + z * q.z;
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

struct HamiltonGateGrads {
    Quat d_g;
    Quat d_u;
};

// Forward pass: r = sqrt(|g|^2 + 1e-12), q = g / r, out = q * u
inline Quat hamilton_gate_forward(const Quat& g, const Quat& u) {
    // TODO: implement Hamilton rotation gate forward pass
    (void)g; (void)u;
    return Quat{};
}

// Backward pass: d_u = conj(q) * d_out, d_q = d_out * conj(u), d_g = (d_q - q * (q . d_q)) / r
inline HamiltonGateGrads hamilton_gate_backward(const Quat& g, const Quat& u, const Quat& d_out) {
    // TODO: implement exact adjoint backward pass
    (void)g; (void)u; (void)d_out;
    return HamiltonGateGrads{};
}
