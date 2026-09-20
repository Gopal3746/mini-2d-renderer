// Milestone 2 demo: exercises the new rasterizer (fill_rect, draw_line)
// on top of the math + framebuffer pipeline from milestone 1.

#include <cmath>
#include <cstdio>

#include "renderer/color.hpp"
#include "renderer/framebuffer.hpp"
#include "renderer/matrix3x3.hpp"
#include "renderer/rasterizer.hpp"
#include "renderer/shapes.hpp"
#include "renderer/vec2.hpp"

using renderer::Color;
using renderer::Framebuffer;
using renderer::Line;
using renderer::Matrix3x3;
using renderer::Rect;
using renderer::Vec2;

int main() {
    const int kWidth = 400;
    const int kHeight = 300;
    Framebuffer fb(kWidth, kHeight);

    // 1. Background gradient, written directly to sanity-check pixel
    //    addressing and Color <-> RGBA8 conversion.
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            const float u = static_cast<float>(x) / (kWidth - 1);
            const float v = static_cast<float>(y) / (kHeight - 1);
            fb.set_pixel_raw(x, y, Color{u, v, 1.0f - u, 1.0f});
        }
    }

    // 2. fill_rect: a semi-transparent white rect over the gradient.
    //    Confirms alpha blending still works when routed through the
    //    rasterizer instead of called pixel-by-pixel by hand.
    renderer::fill_rect(fb, Rect{60, 80, 280, 140}, Color{1.0f, 1.0f, 1.0f, 0.35f});

    // 3. Matrix3x3 + draw_line: transform a unit square's corners, then
    //    connect them with lines to draw the rotated square's outline --
    //    a real use of the rasterizer driven by the transform pipeline,
    //    not just isolated pixel markers.
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

    // Mark each corner too, so it's still easy to see where the vertices
    // actually landed versus where Bresenham drew the connecting edges.
    for (const Vec2& p : transformed) {
        const int px = static_cast<int>(std::lround(p.x));
        const int py = static_cast<int>(std::lround(p.y));
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                fb.set_pixel_raw(px + dx, py + dy, Color{1.0f, 0.0f, 0.0f, 1.0f});
            }
        }
    }

    const char* out_path = "output.png";
    if (!fb.write_png(out_path)) {
        std::fprintf(stderr, "Failed to write %s\n", out_path);
        return 1;
    }
    std::printf("Wrote %s (%dx%d)\n", out_path, kWidth, kHeight);
    return 0;
}
