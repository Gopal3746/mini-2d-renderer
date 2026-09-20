#pragma once

#include <array>
#include <cmath>

#include "renderer/vec2.hpp"

namespace renderer {

// Row-major 3x3 matrix used for 2D affine transforms via homogeneous
// coordinates:
//
//   | m[0] m[1] m[2] |   | x |
//   | m[3] m[4] m[5] | * | y |
//   | m[6] m[7] m[8] |   | 1 |
//
// The bottom row is always (0, 0, 1) for affine transforms, but we keep
// it general so composition is just plain matrix multiplication.
class Matrix3x3 {
public:
    std::array<float, 9> m{1, 0, 0,
                            0, 1, 0,
                            0, 0, 1};

    constexpr Matrix3x3() = default;
    constexpr explicit Matrix3x3(std::array<float, 9> values) : m(values) {}

    static constexpr Matrix3x3 identity() { return Matrix3x3{}; }

    static constexpr Matrix3x3 translate(float tx, float ty) {
        return Matrix3x3{{1, 0, tx,
                           0, 1, ty,
                           0, 0, 1}};
    }
    static constexpr Matrix3x3 translate(const Vec2& t) { return translate(t.x, t.y); }

    static constexpr Matrix3x3 scale(float sx, float sy) {
        return Matrix3x3{{sx, 0, 0,
                           0, sy, 0,
                           0, 0, 1}};
    }
    static constexpr Matrix3x3 scale(float s) { return scale(s, s); }

    // Angle in radians, positive = counter-clockwise in standard math axes
    // (note: with a y-down framebuffer this will look clockwise on screen).
    static Matrix3x3 rotate(float radians) {
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        return Matrix3x3{{c, -s, 0,
                           s, c, 0,
                           0, 0, 1}};
    }

    // Matrix * Matrix (this applied after 'o', i.e. (this * o) * v == this * (o * v))
    [[nodiscard]] constexpr Matrix3x3 operator*(const Matrix3x3& o) const {
        Matrix3x3 out{};
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                float sum = 0.0f;
                for (int k = 0; k < 3; ++k) {
                    sum += m[row * 3 + k] * o.m[k * 3 + col];
                }
                out.m[row * 3 + col] = sum;
            }
        }
        return out;
    }

    // Transform a point (implicit w=1); applies full affine transform including translation.
    [[nodiscard]] constexpr Vec2 apply_point(const Vec2& p) const {
        return Vec2{m[0] * p.x + m[1] * p.y + m[2],
                    m[3] * p.x + m[4] * p.y + m[5]};
    }

    // Transform a direction/vector (implicit w=0); ignores translation.
    [[nodiscard]] constexpr Vec2 apply_vector(const Vec2& v) const {
        return Vec2{m[0] * v.x + m[1] * v.y,
                    m[3] * v.x + m[4] * v.y};
    }
};

}  // namespace renderer
