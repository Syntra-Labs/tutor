#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <utility>

struct Quat {
    double w{0.0};
    double x{0.0};
    double y{0.0};
    double z{0.0};

    Quat() = default;
    Quat(double w_, double x_, double y_, double z_) : w(w_), x(x_), y(y_), z(z_) {}

    Quat mul(const Quat& q) const {
        return Quat(
            w * q.w - x * q.x - y * q.y - z * q.z,
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w
        );
    }
};

// Map clamped score in [0, 1] to table bin index in [0, 15]
inline uint32_t rate_index_of(double score) {
    // TODO: clamp score to [0, 1] and return min(floor(16 * s), 15)
    (void)score;
    return 0;
}

// Return {lambda_i, kappa_i} for s = clamp(g + d, 0, 1)
inline std::pair<double, double> rate_select_forward(double g, double d) {
    // TODO: compute bin i and return {2^(-(i+1)), sqrt(1 - lambda_i^2)}
    (void)g; (void)d;
    return {0.0, 0.0};
}

// Compute secant surrogate gradient dL/ds across neighboring table entries i-1 and i+1
inline double rate_select_secant_backward(double g, double d, double d_lambda, double d_kappa) {
    // TODO: implement secant surrogate credit rule multiplied by 16.0
    (void)g; (void)d; (void)d_lambda; (void)d_kappa;
    return 0.0;
}

// Compute one geometric recurrence step: h_t = lambda_i * (u_unit * h_prev) + kappa_i * b_t
inline Quat recurrence_step_forward(const Quat& h_prev, const Quat& u_unit, const Quat& b_t, double g, double d) {
    // TODO: combine rate_select_forward with quaternion transport u_unit * h_prev
    (void)h_prev; (void)u_unit; (void)b_t; (void)g; (void)d;
    return Quat{};
}
