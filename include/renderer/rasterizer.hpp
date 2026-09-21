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

// Fills a circle with anti-aliased edges. Each pixel near the boundary
// is supersampled on a supersample x supersample subpixel grid; the
// fraction of samples that fall inside the circle becomes that pixel's
// coverage, which scales color.a before blending through
// Framebuffer::set_pixel. Interior and fully-exterior pixels resolve
// to full or zero coverage respectively without needing the full
// sample grid to "look" anti-aliased -- only boundary pixels actually
// end up partially covered.
void fill_circle(Framebuffer& fb, const Circle& circle, const Color& color,
                  int supersample = 4);

}  // namespace renderer
