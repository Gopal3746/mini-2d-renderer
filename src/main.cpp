// Milestone 4 demo: adds Bezier curve flattening + stroking on top of
// rects, lines, and the anti-aliased circle from milestones 1-3.

#include <cmath>
#include <cstdio>

#include "renderer/color.hpp"
#include "renderer/framebuffer.hpp"
#include "renderer/matrix3x3.hpp"
#include "renderer/rasterizer.hpp"
#include "renderer/shapes.hpp"
#include "renderer/vec2.hpp"

using renderer::Circle;
using renderer::Color;
using renderer::CubicBezier;
using renderer::Framebuffer;
using renderer::Line;
using renderer::Matrix3x3;
using renderer::QuadraticBezier;
using renderer::Rect;
using renderer::Vec2;

int main() {
    const int kWidth = 400;
    const int kHeight = 300;
    Framebuffer fb(kWidth, kHeight);

    // 1. Background gradient.
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            const float u = static_cast<float>(x) / (kWidth - 1);
            const float v = static_cast<float>(y) / (kHeight - 1);
            fb.set_pixel_raw(x, y, Color{u, v, 1.0f - u, 1.0f});
        }
    }

    // 2. fill_rect: translucent rect over the gradient.
    renderer::fill_rect(fb, Rect{60, 80, 280, 140}, Color{1.0f, 1.0f, 1.0f, 0.35f});

    // 3. Matrix3x3 + draw_line: rotated square outline.
    const Matrix3x3 xform = Matrix3x3::translate(200.0f, 150.0f) *
                             Matrix3x3::rotate(0.5f) *
                             Matrix3x3::scale(80.0f);

    const Vec2 unit_square[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    Vec2 transformed[4];
    for (int i = 0; i < 4; ++i) {
        transformed[i] = xform.apply_point(unit_square[i]);
    }

    const Color outline_color{0.1f, 0.1f, 0.1f, 1.0f};
    for (int i = 0; i < 4; ++i) {
        const Vec2& a = transformed[i];
        const Vec2& b = transformed[(i + 1) % 4];
        renderer::draw_line(fb, Line{a, b}, outline_color);
    }

    for (const Vec2& p : transformed) {
        const int px = static_cast<int>(std::lround(p.x));
        const int py = static_cast<int>(std::lround(p.y));
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                fb.set_pixel_raw(px + dx, py + dy, Color{1.0f, 0.0f, 0.0f, 1.0f});
            }
        }
    }

    // 4. fill_circle: anti-aliased, straddling the rect's hard edge.
    renderer::fill_circle(fb, Circle{Vec2{330.0f, 200.0f}, 42.0f},
                           Color{0.95f, 0.35f, 0.15f, 0.85f});

    // 5. draw_quadratic_bezier: a gentle upward arc across the open
    //    strip above the rect, entirely on-canvas.
    const QuadraticBezier arc{Vec2{10.0f, 60.0f}, Vec2{200.0f, 10.0f}, Vec2{390.0f, 60.0f}};
    renderer::draw_quadratic_bezier(fb, arc, Color{0.0f, 0.3f, 0.0f, 1.0f});

    // 6. draw_cubic_bezier: a classic S-curve in the open bottom-left
    //    area, clear of both the rect and the circle.
    const CubicBezier s_curve{Vec2{20.0f, 280.0f}, Vec2{140.0f, 280.0f}, Vec2{20.0f, 230.0f},
                               Vec2{140.0f, 230.0f}};
    renderer::draw_cubic_bezier(fb, s_curve, Color{0.5f, 0.0f, 0.5f, 1.0f});

    const char* out_path = "output.png";
    if (!fb.write_png(out_path)) {
        std::fprintf(stderr, "Failed to write %s\n", out_path);
        return 1;
    }
    std::printf("Wrote %s (%dx%d)\n", out_path, kWidth, kHeight);
    return 0;
}
