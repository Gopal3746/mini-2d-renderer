// Milestone: clipping. Adds a scissor-rect-style clip on top of rects,
// lines, the anti-aliased circle, and Bezier curves from earlier steps.

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

    // 5. draw_quadratic_bezier: arc across the top.
    const QuadraticBezier arc{Vec2{10.0f, 60.0f}, Vec2{200.0f, 10.0f}, Vec2{390.0f, 60.0f}};
    renderer::draw_quadratic_bezier(fb, arc, Color{0.0f, 0.3f, 0.0f, 1.0f});

    // 6. draw_cubic_bezier: a classic S-curve in the open bottom-left
    //    area, clear of both the rect and the circle.
    const CubicBezier s_curve{Vec2{20.0f, 280.0f}, Vec2{140.0f, 280.0f}, Vec2{20.0f, 230.0f},
                               Vec2{140.0f, 230.0f}};
    renderer::draw_cubic_bezier(fb, s_curve, Color{0.5f, 0.0f, 0.5f, 1.0f});

    // 7. Clipping: stroke the clip region's boundary first (unclipped,
    //    so the outline itself is fully visible as a reference), then
    //    set that same rect as the clip and fill an oversized circle
    //    that geometrically extends well past it on every side. Only
    //    the portion inside the rect should actually appear -- notably
    //    including the top edge, which would otherwise paint over the
    //    translucent rect's lower-left corner if clipping weren't
    //    working.
    const Rect clip_box{150.0f, 225.0f, 120.0f, 65.0f};  // x:[150,270), y:[225,290)
    const Color clip_outline_color{0.0f, 0.0f, 0.0f, 1.0f};
    {
        const Vec2 tl{clip_box.left(), clip_box.top()};
        const Vec2 tr{clip_box.right(), clip_box.top()};
        const Vec2 br{clip_box.right(), clip_box.bottom()};
        const Vec2 bl{clip_box.left(), clip_box.bottom()};
        renderer::draw_line(fb, Line{tl, tr}, clip_outline_color);
        renderer::draw_line(fb, Line{tr, br}, clip_outline_color);
        renderer::draw_line(fb, Line{br, bl}, clip_outline_color);
        renderer::draw_line(fb, Line{bl, tl}, clip_outline_color);
    }

    fb.set_clip_rect(clip_box);
    renderer::fill_circle(fb, Circle{Vec2{210.0f, 257.0f}, 55.0f},
                           Color{0.1f, 0.6f, 0.9f, 0.9f});
    fb.clear_clip();  // good practice, even though nothing draws after this

    const char* out_path = "output.png";
    if (!fb.write_png(out_path)) {
        std::fprintf(stderr, "Failed to write %s\n", out_path);
        return 1;
    }
    std::printf("Wrote %s (%dx%d)\n", out_path, kWidth, kHeight);
    return 0;
}
