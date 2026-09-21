#pragma once

#include "renderer/vec2.hpp"

namespace renderer {

// Axis-aligned rectangle, defined by its top-left corner and size.
// (Rotation/scale of a rect is handled by transforming its corner points
// with Matrix3x3 before rasterizing, not by this struct.)
struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    constexpr Rect() = default;
    constexpr Rect(float x_, float y_, float w_, float h_)
        : x(x_), y(y_), width(w_), height(h_) {}

    [[nodiscard]] constexpr float left() const { return x; }
    [[nodiscard]] constexpr float right() const { return x + width; }
    [[nodiscard]] constexpr float top() const { return y; }
    [[nodiscard]] constexpr float bottom() const { return y + height; }
};

struct Line {
    Vec2 p0;
    Vec2 p1;

    constexpr Line() = default;
    constexpr Line(Vec2 a, Vec2 b) : p0(a), p1(b) {}
};

struct Circle {
    Vec2 center;
    float radius = 0.0f;

    constexpr Circle() = default;
    constexpr Circle(Vec2 c, float r) : center(c), radius(r) {}
};

struct QuadraticBezier {
    Vec2 p0;
    Vec2 control;
    Vec2 p1;

    constexpr QuadraticBezier() = default;
    constexpr QuadraticBezier(Vec2 p0_, Vec2 c, Vec2 p1_) : p0(p0_), control(c), p1(p1_) {}
};

struct CubicBezier {
    Vec2 p0;
    Vec2 c0;
    Vec2 c1;
    Vec2 p1;

    constexpr CubicBezier() = default;
    constexpr CubicBezier(Vec2 p0_, Vec2 c0_, Vec2 c1_, Vec2 p1_)
        : p0(p0_), c0(c0_), c1(c1_), p1(p1_) {}
};

}  // namespace renderer
