#include "renderer/bezier.hpp"

namespace renderer {

Vec2 evaluate(const QuadraticBezier& curve, float t) {
    const float u = 1.0f - t;
    // B(t) = (1-t)^2 * p0 + 2(1-t)t * control + t^2 * p1
    return curve.p0 * (u * u) + curve.control * (2.0f * u * t) + curve.p1 * (t * t);
}

Vec2 evaluate(const CubicBezier& curve, float t) {
    const float u = 1.0f - t;
    const float uu = u * u;
    const float tt = t * t;
    // B(t) = (1-t)^3*p0 + 3(1-t)^2*t*c0 + 3(1-t)*t^2*c1 + t^3*p1
    return curve.p0 * (uu * u) + curve.c0 * (3.0f * uu * t) + curve.c1 * (3.0f * u * tt) +
           curve.p1 * (tt * t);
}

std::vector<Vec2> flatten(const QuadraticBezier& curve, int segments) {
    std::vector<Vec2> points;
    points.reserve(static_cast<size_t>(segments) + 1);
    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        points.push_back(evaluate(curve, t));
    }
    return points;
}

std::vector<Vec2> flatten(const CubicBezier& curve, int segments) {
    std::vector<Vec2> points;
    points.reserve(static_cast<size_t>(segments) + 1);
    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        points.push_back(evaluate(curve, t));
    }
    return points;
}

}  // namespace renderer
