#include "doctest.h"
#include "renderer/bezier.hpp"

using renderer::CubicBezier;
using renderer::QuadraticBezier;
using renderer::Vec2;

TEST_CASE("Quadratic Bezier passes exactly through its endpoints") {
    QuadraticBezier curve{Vec2{0, 0}, Vec2{50, 100}, Vec2{100, 0}};
    Vec2 start = renderer::evaluate(curve, 0.0f);
    Vec2 end = renderer::evaluate(curve, 1.0f);

    CHECK(start.x == doctest::Approx(0.0f));
    CHECK(start.y == doctest::Approx(0.0f));
    CHECK(end.x == doctest::Approx(100.0f));
    CHECK(end.y == doctest::Approx(0.0f));
}

TEST_CASE("Cubic Bezier passes exactly through its endpoints") {
    CubicBezier curve{Vec2{0, 0}, Vec2{20, 50}, Vec2{80, 50}, Vec2{100, 0}};
    Vec2 start = renderer::evaluate(curve, 0.0f);
    Vec2 end = renderer::evaluate(curve, 1.0f);

    CHECK(start.x == doctest::Approx(0.0f));
    CHECK(start.y == doctest::Approx(0.0f));
    CHECK(end.x == doctest::Approx(100.0f));
    CHECK(end.y == doctest::Approx(0.0f));
}

TEST_CASE("flatten produces segments+1 points including both endpoints") {
    QuadraticBezier curve{Vec2{0, 0}, Vec2{50, 100}, Vec2{100, 0}};
    std::vector<Vec2> points = renderer::flatten(curve, 10);

    CHECK(points.size() == 11);
    CHECK(points.front().x == doctest::Approx(0.0f));
    CHECK(points.back().x == doctest::Approx(100.0f));
}
