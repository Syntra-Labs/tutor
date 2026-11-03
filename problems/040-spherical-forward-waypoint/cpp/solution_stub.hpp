#pragma once

#include <cmath>
#include <vector>

struct Vec3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

enum class Op {
    Forward,
    Turn
};

struct Command {
    Op op;
    double deg;
};

struct SphericalTurtle {
    Vec3 pos{0.0, 0.0, 1.0};
    Vec3 heading{1.0, 0.0, 0.0};
};

// Advance turtle forward along great circle by deg degrees
inline void step_forward(SphericalTurtle& state, double deg) {
    // TODO: rotate (pos, heading) in their great-circle plane and re-orthonormalize
    (void)state; (void)deg;
}

// Rotate tangent heading vector around outward normal pos by deg degrees (+ left, - right)
inline void step_turn(SphericalTurtle& state, double deg) {
    // TODO: rotate heading around pos via Rodrigues' formula: t*cos(a) + (p x t)*sin(a)
    (void)state; (void)deg;
}

// Execute a sequence of Forward/Turn commands starting from pos=(0,0,1), heading=(1,0,0)
inline SphericalTurtle run_spherical_path(const std::vector<Command>& commands) {
    // TODO: iterate through commands and apply step_forward / step_turn
    (void)commands;
    return SphericalTurtle{};
}
