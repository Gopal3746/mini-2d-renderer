#pragma once

#include <cmath>

namespace renderer {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr Vec2() = default;
    constexpr Vec2(float x_, float y_) : x(x_), y(y_) {}

    constexpr Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    constexpr Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    constexpr Vec2 operator*(float s) const { return {x * s, y * s}; }
    constexpr Vec2 operator/(float s) const { return {x / s, y / s}; }

    constexpr Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    constexpr Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    constexpr Vec2& operator*=(float s) { x *= s; y *= s; return *this; }

    constexpr bool operator==(const Vec2& o) const { return x == o.x && y == o.y; }

    [[nodiscard]] constexpr float dot(const Vec2& o) const { return x * o.x + y * o.y; }
    [[nodiscard]] float length() const { return std::sqrt(dot(*this)); }
    [[nodiscard]] Vec2 normalized() const {
        const float len = length();
        return len > 0.0f ? (*this) * (1.0f / len) : Vec2{0.0f, 0.0f};
    }
};

inline constexpr Vec2 operator*(float s, const Vec2& v) { return v * s; }

}  // namespace renderer
