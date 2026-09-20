// Milestone 1 demo: no rasterizer yet. This just proves the pipeline
// (math types -> framebuffer -> alpha blending -> PNG) works end to end.
// Real shape rasterization (rects, lines, circles, beziers) comes next.

#include <cstdio>

#include "renderer/color.hpp"
#include "renderer/framebuffer.hpp"
#include "renderer/matrix3x3.hpp"
#include "renderer/vec2.hpp"

using renderer::Color;
using renderer::Framebuffer;
using renderer::Matrix3x3;
using renderer::Vec2;

int main() {
    const int kWidth = 400;
    const int kHeight = 300;
    Framebuffer fb(kWidth, kHeight);

    // 1. Background gradient, written directly (no blending) to sanity-check
    //    pixel addressing and Color <-> RGBA8 conversion.
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            const float u = static_cast<float>(x) / (kWidth - 1);
            const float v = static_cast<float>(y) / (kHeight - 1);
            fb.set_pixel_raw(x, y, Color{u, v, 1.0f - u, 1.0f});
        }
    }

    // 2. Source-over alpha blending: paint a semi-transparent white square
    //    on top of the gradient and confirm it visibly lightens rather than
    //    replaces the background.
    for (int y = 80; y < 220; ++y) {
        for (int x = 60; x < 340; ++x) {
            fb.set_pixel(x, y, Color{1.0f, 1.0f, 1.0f, 0.35f});
        }
    }

    // 3. Matrix3x3: compose translate * rotate * scale and transform a
    //    handful of points, marking each with an opaque red pixel so we can
    //    see the transform actually did something.
    const Matrix3x3 xform = Matrix3x3::translate(200.0f, 150.0f) *
                             Matrix3x3::rotate(0.5f) *
                             Matrix3x3::scale(80.0f);

    const Vec2 unit_square[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    for (const Vec2& p : unit_square) {
        const Vec2 tp = xform.apply_point(p);
        const int px = static_cast<int>(tp.x);
        const int py = static_cast<int>(tp.y);
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
