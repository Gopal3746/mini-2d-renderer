#pragma once

#include "renderer/color.hpp"
#include "renderer/framebuffer.hpp"
#include "renderer/shapes.hpp"

namespace renderer {

// Fills an axis-aligned rectangle. No anti-aliasing yet: a pixel is
// painted at full weight if its top-left corner falls within
// [left, right) x [top, bottom), otherwise not at all -- hard edges.
// color.a < 1 still alpha-blends against the existing background via
// Framebuffer::set_pixel.
void fill_rect(Framebuffer& fb, const Rect& rect, const Color& color);

// Draws a 1-pixel-wide line with Bresenham's algorithm. No
// anti-aliasing: every pixel the algorithm visits is painted at full
// coverage. Endpoints are rounded to the nearest integer pixel.
void draw_line(Framebuffer& fb, const Line& line, const Color& color);

}  // namespace renderer
