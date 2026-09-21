#include "doctest.h"
#include "renderer/vec2.hpp"

using renderer::Vec2;

TEST_CASE("Vec2 arithmetic operators") {
    Vec2 a{1.0f, 2.0f};
    Vec2 b{3.0f, 4.0f};

    CHECK((a + b).x == doctest::Approx(4.0f));
    CHECK((a + b).y == doctest::Approx(6.0f));
    CHECK((b - a).x == doctest::Approx(2.0f));
    CHECK((b - a).y == doctest::Approx(2.0f));
    CHECK((a * 2.0f).x == doctest::Approx(2.0f));
    CHECK((a * 2.0f).y == doctest::Approx(4.0f));
}

TEST_CASE("Vec2 dot and length") {
    Vec2 a{3.0f, 4.0f};
    CHECK(a.dot(a) == doctest::Approx(25.0f));
    CHECK(a.length() == doctest::Approx(5.0f));  // classic 3-4-5 triangle
}

TEST_CASE("Vec2 normalized has unit length, zero vector stays zero") {
    Vec2 a{3.0f, 4.0f};
    Vec2 n = a.normalized();
    CHECK(n.length() == doctest::Approx(1.0f));

    Vec2 zero{0.0f, 0.0f};
    Vec2 normalized_zero = zero.normalized();
    CHECK(normalized_zero.x == doctest::Approx(0.0f));
    CHECK(normalized_zero.y == doctest::Approx(0.0f));
}
