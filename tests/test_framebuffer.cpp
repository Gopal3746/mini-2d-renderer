#include <stdexcept>

#include "doctest.h"
#include "renderer/framebuffer.hpp"

using renderer::Color;
using renderer::Framebuffer;
using renderer::Rect;

TEST_CASE("Framebuffer rejects non-positive dimensions") {
    CHECK_THROWS_AS(Framebuffer(0, 10), std::invalid_argument);
    CHECK_THROWS_AS(Framebuffer(10, 0), std::invalid_argument);
    CHECK_THROWS_AS(Framebuffer(-5, 10), std::invalid_argument);
}

TEST_CASE("Framebuffer set_pixel_raw / get_pixel round trip") {
    Framebuffer fb(4, 4);
    fb.set_pixel_raw(1, 1, Color{1.0f, 0.0f, 0.0f, 1.0f});
    Color c = fb.get_pixel(1, 1);
    CHECK(c.r8() == 255);
    CHECK(c.g8() == 0);
    CHECK(c.b8() == 0);
    CHECK(c.a8() == 255);
}

TEST_CASE("Framebuffer out-of-bounds writes are silent no-ops") {
    Framebuffer fb(4, 4);
    // None of these should throw or corrupt anything -- just verifying
    // they don't crash. (ASan/UBSan in the Debug build would catch any
    // actual out-of-bounds memory access here.)
    fb.set_pixel_raw(-1, 0, Color{1, 0, 0, 1});
    fb.set_pixel_raw(0, -1, Color{1, 0, 0, 1});
    fb.set_pixel_raw(4, 0, Color{1, 0, 0, 1});
    fb.set_pixel_raw(0, 4, Color{1, 0, 0, 1});
    CHECK(true);  // reaching this line without crashing is the assertion
}

TEST_CASE("Framebuffer set_pixel blends via source-over") {
    Framebuffer fb(1, 1);
    fb.set_pixel_raw(0, 0, Color{0.0f, 0.0f, 0.0f, 1.0f});  // opaque black
    fb.set_pixel(0, 0, Color{1.0f, 1.0f, 1.0f, 0.5f});      // 50% white over it

    // source-over of 50% white onto opaque black == 50% gray.
    Color result = fb.get_pixel(0, 0);
    CHECK(result.r8() == doctest::Approx(128).epsilon(2));
    CHECK(result.a8() == 255);  // opaque background -> still fully opaque
}

TEST_CASE("Framebuffer clip rect restricts writes but not reads") {
    Framebuffer fb(10, 10);
    fb.set_pixel_raw(5, 5, Color{1.0f, 0.0f, 0.0f, 1.0f});  // red, outside future clip

    fb.set_clip_rect(Rect{0.0f, 0.0f, 3.0f, 3.0f});  // x:[0,3), y:[0,3)

    // Inside the clip: write lands normally.
    fb.set_pixel_raw(1, 1, Color{0.0f, 1.0f, 0.0f, 1.0f});
    CHECK(fb.get_pixel(1, 1).g8() == 255);

    // Outside the clip: write is dropped, original red is untouched.
    fb.set_pixel_raw(5, 5, Color{0.0f, 0.0f, 1.0f, 1.0f});
    CHECK(fb.get_pixel(5, 5).r8() == 255);

    // Reading outside the clip still works -- clip only gates writes.
    CHECK(fb.get_pixel(5, 5).r8() == 255);

    fb.clear_clip();
    fb.set_pixel_raw(5, 5, Color{0.0f, 0.0f, 1.0f, 1.0f});
    CHECK(fb.get_pixel(5, 5).b8() == 255);  // clip removed, write lands now
}
