#include "doctest.h"
#include "renderer/matrix3x3.hpp"
#include "renderer/vec2.hpp"

using renderer::Matrix3x3;
using renderer::Vec2;

namespace {
constexpr float kPi = 3.14159265358979323846f;
}

TEST_CASE("Matrix3x3 identity leaves points unchanged") {
    Matrix3x3 m = Matrix3x3::identity();
    Vec2 result = m.apply_point(Vec2{5.0f, -3.0f});
    CHECK(result.x == doctest::Approx(5.0f));
    CHECK(result.y == doctest::Approx(-3.0f));
}

TEST_CASE("Matrix3x3 translate moves points but not vectors") {
    Matrix3x3 m = Matrix3x3::translate(10.0f, 20.0f);

    Vec2 moved = m.apply_point(Vec2{1.0f, 1.0f});
    CHECK(moved.x == doctest::Approx(11.0f));
    CHECK(moved.y == doctest::Approx(21.0f));

    // apply_vector ignores translation -- a direction shouldn't move just
    // because the matrix also happens to carry a translation component.
    Vec2 unmoved = m.apply_vector(Vec2{1.0f, 1.0f});
    CHECK(unmoved.x == doctest::Approx(1.0f));
    CHECK(unmoved.y == doctest::Approx(1.0f));
}

TEST_CASE("Matrix3x3 scale") {
    Matrix3x3 m = Matrix3x3::scale(2.0f, 3.0f);
    Vec2 result = m.apply_point(Vec2{1.0f, 1.0f});
    CHECK(result.x == doctest::Approx(2.0f));
    CHECK(result.y == doctest::Approx(3.0f));
}

TEST_CASE("Matrix3x3 rotate 90 degrees maps (1,0) to (0,1)") {
    Matrix3x3 m = Matrix3x3::rotate(kPi / 2.0f);
    Vec2 result = m.apply_point(Vec2{1.0f, 0.0f});
    CHECK(result.x == doctest::Approx(0.0f).epsilon(0.001));
    CHECK(result.y == doctest::Approx(1.0f).epsilon(0.001));
}

TEST_CASE("Matrix3x3 composition applies the right-hand operand first") {
    // translate * scale should scale first, then translate. If composition
    // order were backwards, the origin would land at (10*5, 0) instead of
    // (10, 0) -- this test would catch that regression.
    Matrix3x3 m = Matrix3x3::translate(10.0f, 0.0f) * Matrix3x3::scale(5.0f, 5.0f);
    Vec2 result = m.apply_point(Vec2{0.0f, 0.0f});
    CHECK(result.x == doctest::Approx(10.0f));
    CHECK(result.y == doctest::Approx(0.0f));
}
