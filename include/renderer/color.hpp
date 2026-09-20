#pragma once

#include <algorithm>
#include <cstdint>

namespace renderer {

// Straight (non-premultiplied) alpha, components in [0, 1].
struct Color {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;

    constexpr Color() = default;
    constexpr Color(float r_, float g_, float b_, float a_ = 1.0f)
        : r(r_), g(g_), b(b_), a(a_) {}

    static constexpr Color from_rgba8(uint8_t r8, uint8_t g8, uint8_t b8, uint8_t a8 = 255) {
        return Color{r8 / 255.0f, g8 / 255.0f, b8 / 255.0f, a8 / 255.0f};
    }

    [[nodiscard]] constexpr uint8_t r8() const { return to_u8(r); }
    [[nodiscard]] constexpr uint8_t g8() const { return to_u8(g); }
    [[nodiscard]] constexpr uint8_t b8() const { return to_u8(b); }
    [[nodiscard]] constexpr uint8_t a8() const { return to_u8(a); }

private:
    static constexpr uint8_t to_u8(float v) {
        v = v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
        return static_cast<uint8_t>(v * 255.0f + 0.5f);
    }
};

}  // namespace renderer
