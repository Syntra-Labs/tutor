#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

struct Quat {
    double w{1.0};
    double x{0.0};
    double y{0.0};
    double z{0.0};

    Quat() = default;
    Quat(double w_, double x_, double y_, double z_) : w(w_), x(x_), y(y_), z(z_) {}

    static Quat identity() { return Quat(1.0, 0.0, 0.0, 0.0); }

    double norm() const { return std::sqrt(w * w + x * x + y * y + z * z); }

    Quat normalize() const {
        double n = norm();
        return (n < 1e-12) ? identity() : Quat(w / n, x / n, y / n, z / n);
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

    Quat rotate(const Quat& r) const {
        return r.mul(*this).mul(r.conj());
    }

    double geodesic_dist(const Quat& q) const {
        double d = std::abs(w * q.w + x * q.x + y * q.y + z * q.z);
        if (d > 1.0) d = 1.0;
        return 2.0 * std::acos(d);
    }
};

inline Quat make_rotor(double angle_rad, double ux, double uy, double uz) {
    double norm_u = std::sqrt(ux * ux + uy * uy + uz * uz);
    if (norm_u < 1e-12) return Quat::identity();
    double half = angle_rad * 0.5;
    double s = std::sin(half) / norm_u;
    return Quat(std::cos(half), s * ux, s * uy, s * uz).normalize();
}

inline uint64_t fnv1a_64_str(const std::string& s) {
    uint64_t h = 0xCBF29CE484222325ULL;
    for (unsigned char c : s) {
        h ^= static_cast<uint64_t>(c);
        h *= 0x100000001B3ULL;
    }
    return h;
}

enum class ExprKind : uint8_t {
    BVAR = 0,
    FVAR = 1,
    MVAR = 2,
    SORT = 3,
    CONST = 4,
    APP = 5,
    LAM = 6,
    FORALLE = 7,
    LETE = 8,
    LIT = 9,
    MDATA = 10
};

struct ExprNode {
    uint32_t id = 0;
    ExprKind kind = ExprKind::BVAR;
    uint64_t payload = 0;
    std::string symbol{};
    std::vector<uint32_t> children{};
};

struct RotorCodebook {
    Quat constr_rotors[11];
    Quat r_left;
    Quat r_right;
    Quat r_dom;
    Quat r_body;
    Quat r_let_type;
    Quat r_let_val;
    Quat r_let_body;

    RotorCodebook() {
        constexpr double pi = 3.14159265358979323846;
        for (int i = 0; i < 11; ++i) {
            double theta = (i + 1) * (2.0 * pi / 13.0);
            constr_rotors[i] = make_rotor(theta, std::cos(theta * 1.5), std::sin(theta * 2.1), std::cos(theta * 0.7));
        }
        r_left     = make_rotor(pi / 3.0, 1.0, 0.0, 0.0);
        r_right    = make_rotor(pi / 4.0, 0.0, 1.0, 0.0);
        r_dom      = make_rotor(pi / 5.0, 0.0, 0.0, 1.0);
        r_body     = make_rotor(2.0 * pi / 5.0, 1.0, 1.0, 0.0);
        r_let_type = make_rotor(pi / 6.0, 1.0, 0.0, 1.0);
        r_let_val  = make_rotor(pi / 7.0, 0.0, 1.0, 1.0);
        r_let_body = make_rotor(pi / 8.0, 1.0, 1.0, 1.0);
    }
};

class TreeRotorOperator {
public:
    explicit TreeRotorOperator(RotorCodebook cb = RotorCodebook()) : codebook_(cb) {}

    Quat fold_dag(
        const std::vector<ExprNode>& nodes,
        uint32_t root_id,
        size_t* out_visited_count = nullptr
    ) const {
        // TODO: implement memoized bottom-up non-commutative rotor folding on S^3
        (void)nodes;
        (void)root_id;
        if (out_visited_count) *out_visited_count = 0;
        return Quat::identity();
    }

    double non_commutative_asymmetry(const ExprNode& a, const ExprNode& b) const {
        ExprNode app_ab{100, ExprKind::APP, 0, "", {a.id, b.id}};
        ExprNode app_ba{101, ExprKind::APP, 0, "", {b.id, a.id}};
        Quat q_ab = fold_dag({a, b, app_ab}, 100);
        Quat q_ba = fold_dag({a, b, app_ba}, 101);
        return q_ab.geodesic_dist(q_ba);
    }

private:
    RotorCodebook codebook_;
};
