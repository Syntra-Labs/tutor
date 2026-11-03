#pragma once

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>

struct Rational {
    int64_t num = 0;
    int64_t den = 1;

    Rational() = default;
    Rational(int64_t n, int64_t d = 1) {
        if (d < 0) { n = -n; d = -d; }
        if (n == 0) {
            num = 0;
            den = 1;
        } else {
            int64_t g = std::gcd(n < 0 ? -n : n, d);
            num = n / g;
            den = d / g;
        }
    }

    bool is_zero() const { return num == 0; }
    bool is_positive() const { return num > 0; }
    bool is_negative() const { return num < 0; }

    Rational operator+(const Rational& o) const {
        return Rational(num * o.den + o.num * den, den * o.den);
    }
    Rational operator-(const Rational& o) const {
        return Rational(num * o.den - o.num * den, den * o.den);
    }
    Rational operator*(const Rational& o) const {
        return Rational(num * o.num, den * o.den);
    }
    Rational operator/(const Rational& o) const {
        return Rational(num * o.den, den * o.num);
    }
    bool operator==(const Rational& o) const {
        return num == o.num && den == o.den;
    }
    bool operator<(const Rational& o) const {
        return (__int128)num * o.den < (__int128)o.num * den;
    }
    bool operator<=(const Rational& o) const {
        return (__int128)num * o.den <= (__int128)o.num * den;
    }
};

struct SimplexTableau {
    std::vector<std::vector<Rational>> t; // m x width matrix
    std::vector<Rational> rhs;            // m right-hand sides (>= 0)
    std::vector<size_t> basis;            // m basic column indices
    std::vector<Rational> d;              // width reduced costs

    void pivot(size_t r, size_t j) {
        // TODO: perform exact rational Gauss-Jordan pivot on row r and entering column j
        (void)r;
        (void)j;
    }

    // Perform one Bland's rule pivot step over columns [0, allowed_cols).
    // Returns true if a pivot was performed, or false if optimal (no negative reduced cost) or unbounded.
    bool step_blands_rule(size_t allowed_cols) {
        // TODO: select smallest j with d[j] < 0 and leaving row r minimizing rhs[i]/t[i][j] (tie -> smallest basis[i])
        (void)allowed_cols;
        return false;
    }
};

inline bool verify_lp_certificate(
    const std::vector<std::vector<Rational>>& A,
    const std::vector<Rational>& b,
    const std::vector<Rational>& c,
    const std::vector<Rational>& x_primal,
    const std::vector<Rational>& y_dual
) {
    // TODO: verify Ax == b, x >= 0, A^T y <= c, and c^T x == b^T y in exact rational arithmetic
    (void)A;
    (void)b;
    (void)c;
    (void)x_primal;
    (void)y_dual;
    return false;
}
