#pragma once

#include <algorithm>
#include <cmath>

inline constexpr double SPHERE_PI = 3.14159265358979323846;

// Convert degrees to radians
inline double deg_to_rad(double deg) {
    // TODO: implement degree-to-radian conversion
    (void)deg;
    return 0.0;
}

// Compute great-circle central angle in radians in [0, pi] using clamped Haversine formula
inline double haversine_distance_rad(double lat1_deg, double lon1_deg, double lat2_deg, double lon2_deg) {
    // TODO: implement clamped Haversine formula
    (void)lat1_deg; (void)lon1_deg; (void)lat2_deg; (void)lon2_deg;
    return 0.0;
}

// Compute great-circle central angle in degrees in [0, 180]
inline double haversine_distance_deg(double lat1_deg, double lon1_deg, double lat2_deg, double lon2_deg) {
    // TODO: convert haversine_distance_rad to degrees
    (void)lat1_deg; (void)lon1_deg; (void)lat2_deg; (void)lon2_deg;
    return 0.0;
}
