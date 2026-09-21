#pragma once

#include <vector>

#include "renderer/shapes.hpp"
#include "renderer/vec2.hpp"

namespace renderer {

// Evaluates the curve at parameter t in [0, 1] via the standard Bernstein
// polynomial form (equivalent to De Casteljau's algorithm, just expanded
// out algebraically instead of computed by repeated linear interpolation).
[[nodiscard]] Vec2 evaluate(const QuadraticBezier& curve, float t);
[[nodiscard]] Vec2 evaluate(const CubicBezier& curve, float t);

// Flattens a curve into a polyline: `segments + 1` points sampled at
// evenly-spaced t values, ready to be connected with straight lines.
//
// Real rasterizers use *adaptive* subdivision -- more segments where the
// curve bends sharply, fewer where it's nearly straight, based on a
// flatness tolerance -- so they don't waste segments on flat stretches or
// under-sample tight curves. This fixed-step version always produces the
// same segment count regardless of how curved the path actually is; it's
// simpler to reason about and plenty for the curve sizes this renderer
// draws, at the cost of being less adaptive than a production
// implementation.
[[nodiscard]] std::vector<Vec2> flatten(const QuadraticBezier& curve, int segments = 24);
[[nodiscard]] std::vector<Vec2> flatten(const CubicBezier& curve, int segments = 32);

}  // namespace renderer
