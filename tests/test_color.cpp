#include "doctest.h"
#include "renderer/color.hpp"

using renderer::Color;

TEST_CASE("Color from_rgba8 / to_u8 round trip") {
    Color c = Color::from_rgba8(255, 128, 0, 64);
    CHECK(c.r8() == 255);
    CHECK(c.g8() == 128);
    CHECK(c.b8() == 0);
    CHECK(c.a8() == 64);
}

TEST_CASE("Color clamps out-of-range float components") {
    Color c{1.5f, -0.5f, 0.5f, 2.0f};
    CHECK(c.r8() == 255);  // clamped down from 1.5
    CHECK(c.g8() == 0);    // clamped up from -0.5
    CHECK(c.a8() == 255);  // clamped down from 2.0
}
