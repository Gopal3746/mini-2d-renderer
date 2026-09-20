#include "renderer/framebuffer.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace renderer {

Framebuffer::Framebuffer(int width, int height) : width_(width), height_(height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Framebuffer dimensions must be positive");
    }
    pixels_.assign(static_cast<size_t>(width) * height * 4, 0);
}


void Framebuffer::clear(const Color& color) {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            write_rgba8(index(x, y), color);
        }
    }
}

void Framebuffer::write_rgba8(size_t idx, const Color& color) {
    pixels_[idx + 0] = color.r8();
    pixels_[idx + 1] = color.g8();
    pixels_[idx + 2] = color.b8();
    pixels_[idx + 3] = color.a8();
}

void Framebuffer::set_pixel_raw(int x, int y, const Color& color) {
    if (!in_bounds(x, y)) return;
    write_rgba8(index(x, y), color);
}

void Framebuffer::set_pixel(int x, int y, const Color& color) {
    if (!in_bounds(x, y)) return;
    const size_t i = index(x, y);

    if (color.a >= 1.0f) {
        write_rgba8(i, color);
        return;
    }
    if (color.a <= 0.0f) {
        return;
    }

    const Color dst = get_pixel(x, y);
    const float out_a = color.a + dst.a * (1.0f - color.a);

    Color out;
    if (out_a > 0.0f) {
        out.r = (color.r * color.a + dst.r * dst.a * (1.0f - color.a)) / out_a;
        out.g = (color.g * color.a + dst.g * dst.a * (1.0f - color.a)) / out_a;
        out.b = (color.b * color.a + dst.b * dst.a * (1.0f - color.a)) / out_a;
    }
    out.a = out_a;

    write_rgba8(i, out);
}

Color Framebuffer::get_pixel(int x, int y) const {
    if (!in_bounds(x, y)) return Color{0, 0, 0, 0};
    const size_t i = index(x, y);
    return Color::from_rgba8(pixels_[i + 0], pixels_[i + 1], pixels_[i + 2], pixels_[i + 3]);
}

bool Framebuffer::write_png(const std::string& path) const {
    const int stride_bytes = width_ * 4;
    return stbi_write_png(path.c_str(), width_, height_, 4, pixels_.data(), stride_bytes) != 0;
}

}  // namespace renderer
