#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

struct Vec3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

// Compute the interior spherical angle in [0, pi] at vertex b between arcs b->a and b->c
inline double spherical_triangle_angle(const Vec3& a, const Vec3& b, const Vec3& c) {
    // TODO: project a and c onto the tangent plane at b and compute angle between them
    (void)a; (void)b; (void)c;
    return 0.0;
}

// Compute the spherical excess (area on unit S^2) of geodesic triangle (a, b, c)
inline double spherical_excess_triangle(const Vec3& a, const Vec3& b, const Vec3& c) {
    // TODO: sum interior angles at a, b, c minus pi
    (void)a; (void)b; (void)c;
    return 0.0;
}

// Compute the spherical excess of a convex geodesic n-gon with vertices in cyclic order
inline double spherical_excess_polygon(const std::vector<Vec3>& vertices) {
    // TODO: sum interior angles at all n vertices minus (n - 2) * pi
    (void)vertices;
    return 0.0;
}
