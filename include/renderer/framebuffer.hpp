#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "renderer/color.hpp"
#include "renderer/shapes.hpp"

namespace renderer {

// An RGBA8 framebuffer. Pixel (0, 0) is the top-left corner.
class Framebuffer {
public:
    // Throws std::invalid_argument if width or height is not positive.
    Framebuffer(int width, int height);

    [[nodiscard]] int width() const { return width_; }
    [[nodiscard]] int height() const { return height_; }

    // Clears the ENTIRE buffer to `color`, ignoring any active clip rect.
    // (Real APIs sometimes respect scissor on clear; this one always
    // clears everything, to keep the mental model simple: clip only
    // gates the per-pixel drawing calls below, nothing else.)
    void clear(const Color& color);

    // Restricts set_pixel/set_pixel_raw to this rectangle in addition to
    // the framebuffer's own bounds. Edges are rounded the same way
    // fill_rect rounds a rect (ceil, half-open interval), then clamped to
    // [0, width) x [0, height) -- a clip can only ever restrict drawing
    // further, never expand it past the buffer's actual size.
    void set_clip_rect(const Rect& rect);

    // Removes any clip restriction; drawing is limited only by the
    // framebuffer's own bounds again.
    void clear_clip();

    // The currently active clip rectangle, in pixel coordinates. Equals
    // the full framebuffer if no clip has been set.
    [[nodiscard]] Rect clip_rect() const;

    // Source-over blend of `color` into the pixel at (x, y). Coordinates
    // outside the framebuffer's own bounds, or outside the active clip
    // rect if one is set, are silently ignored.
    void set_pixel(int x, int y, const Color& color);

    // Overwrites the pixel with no blending. Same out-of-bounds/clip
    // behavior as set_pixel.
    void set_pixel_raw(int x, int y, const Color& color);

    // Reads a pixel's current color. NOT affected by the clip rect --
    // clipping only gates writes, the same way a scissor test in a real
    // graphics API only affects fragment output, not arbitrary reads.
    [[nodiscard]] Color get_pixel(int x, int y) const;

    [[nodiscard]] const uint8_t* data() const { return pixels_.data(); }
    [[nodiscard]] size_t size_bytes() const { return pixels_.size(); }

    // Writes the framebuffer out as an 8-bit RGBA PNG. Returns true on success.
    [[nodiscard]] bool write_png(const std::string& path) const;

private:
    int width_;
    int height_;
    std::vector<uint8_t> pixels_;  // RGBA8, row-major, 4 bytes/pixel

    // Active clip rect in integer pixel bounds, always clamped to
    // [0, width_] x [0, height_]. Defaults to the full framebuffer.
    int clip_x0_ = 0;
    int clip_y0_ = 0;
    int clip_x1_ = 0;
    int clip_y1_ = 0;

    // True framebuffer bounds, ignoring clip. Used by get_pixel, which
    // reads are not subject to clipping.
    [[nodiscard]] bool in_bounds(int x, int y) const {
        return x >= 0 && y >= 0 && x < width_ && y < height_;
    }
    // Clip bounds -- since clip_{x,y}{0,1}_ are always clamped to the
    // framebuffer's own bounds, passing this check implies in_bounds()
    // would also pass, so writers only need to call this one.
    [[nodiscard]] bool in_clip(int x, int y) const {
        return x >= clip_x0_ && y >= clip_y0_ && x < clip_x1_ && y < clip_y1_;
    }
    [[nodiscard]] size_t index(int x, int y) const {
        return (static_cast<size_t>(y) * width_ + x) * 4;
    }
    // Writes at a pre-computed index with NO bounds/clip check -- callers
    // must have already validated (x, y) themselves. Exists so set_pixel
    // can do its check exactly once instead of once itself and again
    // inside set_pixel_raw.
    void write_rgba8(size_t idx, const Color& color);
};

}  // namespace renderer
