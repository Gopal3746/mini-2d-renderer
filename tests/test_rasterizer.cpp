#include <cstdint>

#include "doctest.h"
#include "renderer/rasterizer.hpp"

using renderer::Circle;
using renderer::Color;
using renderer::Framebuffer;
using renderer::Line;
using renderer::Rect;
using renderer::Vec2;

TEST_CASE("fill_rect paints exactly its half-open region") {
    Framebuffer fb(10, 10);
    renderer::fill_rect(fb, Rect{2, 2, 3, 3}, Color{1, 1, 1, 1});  // x:[2,5), y:[2,5)

    CHECK(fb.get_pixel(2, 2).a8() == 255);  // top-left corner: inside
    CHECK(fb.get_pixel(4, 4).a8() == 255);  // bottom-right-most pixel: inside
    CHECK(fb.get_pixel(5, 4).a8() == 0);    // one past the right edge
    CHECK(fb.get_pixel(4, 5).a8() == 0);    // one past the bottom edge
    CHECK(fb.get_pixel(1, 2).a8() == 0);    // one before the left edge
}

TEST_CASE("draw_line paints both endpoints") {
    Framebuffer fb(10, 10);
    renderer::draw_line(fb, Line{Vec2{1, 1}, Vec2{6, 4}}, Color{1, 1, 1, 1});
    CHECK(fb.get_pixel(1, 1).a8() == 255);
    CHECK(fb.get_pixel(6, 4).a8() == 255);
}

TEST_CASE("fill_circle: center is fully covered, far corner untouched") {
    Framebuffer fb(40, 40);
    renderer::fill_circle(fb, Circle{Vec2{20, 20}, 10}, Color{1, 1, 1, 1});

    CHECK(fb.get_pixel(20, 20).a8() == 255);  // dead center: full coverage
    CHECK(fb.get_pixel(0, 0).a8() == 0);      // corner, nowhere near the circle
}

TEST_CASE("fill_circle: at least one boundary pixel gets partial coverage") {
    Framebuffer fb(40, 40);
    renderer::fill_circle(fb, Circle{Vec2{20, 20}, 10}, Color{1, 1, 1, 1});

    // This is the actual point of anti-aliasing: some pixel straddling the
    // circle's edge should have neither full nor zero alpha. Scanning the
    // whole buffer rather than guessing one coordinate by hand keeps this
    // test robust to exactly where the supersampling grid lands.
    bool found_partial = false;
    for (int y = 0; y < fb.height() && !found_partial; ++y) {
        for (int x = 0; x < fb.width(); ++x) {
            const uint8_t a = fb.get_pixel(x, y).a8();
            if (a > 0 && a < 255) {
                found_partial = true;
                break;
            }
        }
    }
    CHECK(found_partial);
}
