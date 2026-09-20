#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "renderer/color.hpp"

namespace renderer {

// An RGBA8 framebuffer. Pixel (0, 0) is the top-left corner.
class Framebuffer {
public:
    // Throws std::invalid_argument if width or height is not positive.
    Framebuffer(int width, int height);

    [[nodiscard]] int width() const { return width_; }
    [[nodiscard]] int height() const { return height_; }

    void clear(const Color& color);

    // Source-over blend of `color` into the pixel at (x, y). Out-of-bounds
    // coordinates are silently ignored (this is the framebuffer's implicit
    // clip to its own bounds; a separate clip-rect comes in a later step).
    void set_pixel(int x, int y, const Color& color);

    // Overwrites the pixel with no blending. Out-of-bounds is a no-op.
    void set_pixel_raw(int x, int y, const Color& color);

    [[nodiscard]] Color get_pixel(int x, int y) const;

    [[nodiscard]] const uint8_t* data() const { return pixels_.data(); }
    [[nodiscard]] size_t size_bytes() const { return pixels_.size(); }

    // Writes the framebuffer out as an 8-bit RGBA PNG. Returns true on success.
    [[nodiscard]] bool write_png(const std::string& path) const;

private:
    int width_;
    int height_;
    std::vector<uint8_t> pixels_;  // RGBA8, row-major, 4 bytes/pixel

    [[nodiscard]] bool in_bounds(int x, int y) const {
        return x >= 0 && y >= 0 && x < width_ && y < height_;
    }
    [[nodiscard]] size_t index(int x, int y) const {
        return (static_cast<size_t>(y) * width_ + x) * 4;
    }
    // Writes at a pre-computed index with NO bounds check -- callers must
    // have already validated (x, y) themselves. Exists so set_pixel can do
    // its bounds check exactly once instead of once itself and again inside
    // set_pixel_raw.
    void write_rgba8(size_t idx, const Color& color);
};

}  // namespace renderer
