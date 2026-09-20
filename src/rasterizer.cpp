#include "renderer/rasterizer.hpp"

#include <cmath>
#include <cstdlib>

namespace renderer {

void fill_rect(Framebuffer& fb, const Rect& rect, const Color& color) {
    // Round each edge outward to the nearest pixel boundary and iterate a
    // half-open [x0, x1) x [y0, y1) range, matching how most 2D APIs treat
    // rect edges. This is a hard-edged fill -- a pixel is either fully
    // painted or not touched at all. Partial-coverage edges are the
    // anti-aliasing milestone's job, not this one's.
    const int x0 = static_cast<int>(std::ceil(rect.left()));
    const int y0 = static_cast<int>(std::ceil(rect.top()));
    const int x1 = static_cast<int>(std::ceil(rect.right()));
    const int y1 = static_cast<int>(std::ceil(rect.bottom()));

    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            fb.set_pixel(x, y, color);
        }
    }
}

void draw_line(Framebuffer& fb, const Line& line, const Color& color) {
    int x0 = static_cast<int>(std::lround(line.p0.x));
    int y0 = static_cast<int>(std::lround(line.p0.y));
    const int x1 = static_cast<int>(std::lround(line.p1.x));
    const int y1 = static_cast<int>(std::lround(line.p1.y));

    const int dx = std::abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;
    const int dy = -std::abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        fb.set_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        const int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

}  // namespace renderer
