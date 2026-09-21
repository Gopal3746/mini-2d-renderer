#pragma once

#include <cstddef>
#include <cstdint>

namespace renderer {

// Composites `count` RGBA8 pixels of `src` over `dst`, writing the result
// to `out` (which may alias dst for in-place blending). Each buffer must
// hold at least count*4 interleaved RGBA bytes.
//
// This is a DIFFERENT, narrower blend than Framebuffer::set_pixel's:
//   - Assumes dst is fully opaque (alpha = 255) and out stays opaque.
//     This is the common fast path for compositing onto an opaque
//     framebuffer -- Framebuffer::set_pixel handles the more general
//     (and more expensive) case of a semi-transparent destination,
//     which this renderer's rasterizer actually needs when shapes
//     overlap mid-scene.
//   - Uses integer math with an exact (not approximate) divide-by-255
//     via the classic Blinn trick, rather than a float division. See
//     blend.cpp's div255() for why a naive ">>8" shift is NOT the same
//     thing and is actually biased, not just imprecise.
// It exists specifically to give scalar and NEON something identical
// and simple enough to compare fairly: any timing difference between
// blend_over_scalar and blend_over_neon reflects real SIMD parallelism,
// not a difference in what's being computed.
void blend_over_scalar(const uint8_t* src, const uint8_t* dst, uint8_t* out, size_t count);

#if RENDERER_HAS_NEON
// Same contract and same integer approximation as blend_over_scalar,
// implemented with ARM NEON intrinsics, processing 16 pixels per
// iteration (plus a scalar tail for any remainder). RENDERER_HAS_NEON
// is defined by CMake (0 or 1) based on the detected target
// architecture -- this function does not exist in the build at all
// when it's 0, rather than existing-but-crashing on the wrong CPU.
void blend_over_neon(const uint8_t* src, const uint8_t* dst, uint8_t* out, size_t count);
#endif

}  // namespace renderer
