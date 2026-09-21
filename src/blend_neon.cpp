#include "renderer/blend.hpp"

#if RENDERER_HAS_NEON

#include <arm_neon.h>

namespace renderer {

namespace {

// Vectorized version of the same div255() trick used in blend.cpp,
// operating on 8 lanes of uint16 at once. Must match that scalar
// sequence step for step -- this is what makes blend_over_neon's output
// provably identical to blend_over_scalar's, not just close to it.
inline uint16x8_t div255(uint16x8_t x) {
    const uint16x8_t t = vaddq_u16(x, vdupq_n_u16(128));
    return vshrq_n_u16(vaddq_u16(t, vshrq_n_u16(t, 8)), 8);
}

// Blends one 16-lane uint8 channel: (src*a + dst*inv_a), divided exactly
// by 255. vmull_u8 widens each 8-lane uint8 half to a full uint16x8
// product (no truncation -- uint8*uint8 maxes at 65025, comfortably
// within uint16's 65535), matching the bound already established in
// blend.cpp for the scalar path.
inline uint8x16_t blend_channel(uint8x16_t src_c, uint8x16_t dst_c, uint8x16_t alpha,
                                 uint8x16_t inv_alpha) {
    const uint16x8_t term1_lo = vmull_u8(vget_low_u8(src_c), vget_low_u8(alpha));
    const uint16x8_t term2_lo = vmull_u8(vget_low_u8(dst_c), vget_low_u8(inv_alpha));
    const uint16x8_t sum_lo = vaddq_u16(term1_lo, term2_lo);
    const uint8x8_t out_lo = vqmovn_u16(div255(sum_lo));

    const uint16x8_t term1_hi = vmull_u8(vget_high_u8(src_c), vget_high_u8(alpha));
    const uint16x8_t term2_hi = vmull_u8(vget_high_u8(dst_c), vget_high_u8(inv_alpha));
    const uint16x8_t sum_hi = vaddq_u16(term1_hi, term2_hi);
    const uint8x8_t out_hi = vqmovn_u16(div255(sum_hi));

    return vcombine_u8(out_lo, out_hi);
}

}  // namespace

void blend_over_neon(const uint8_t* src, const uint8_t* dst, uint8_t* out, size_t count) {
    size_t i = 0;

    for (; i + 16 <= count; i += 16) {
        // vld4q_u8 loads 16 interleaved RGBA pixels (64 bytes) and
        // deinterleaves them into four uint8x16_t vectors, one per
        // channel -- exactly the layout blend_channel wants, with no
        // manual shuffling needed.
        const uint8x16x4_t s = vld4q_u8(src + i * 4);
        const uint8x16x4_t d = vld4q_u8(dst + i * 4);
        const uint8x16_t alpha = s.val[3];
        const uint8x16_t inv_alpha = vsubq_u8(vdupq_n_u8(255), alpha);

        uint8x16x4_t o;
        o.val[0] = blend_channel(s.val[0], d.val[0], alpha, inv_alpha);
        o.val[1] = blend_channel(s.val[1], d.val[1], alpha, inv_alpha);
        o.val[2] = blend_channel(s.val[2], d.val[2], alpha, inv_alpha);
        o.val[3] = vdupq_n_u8(255);  // dst assumed opaque; stays opaque

        vst4q_u8(out + i * 4, o);
    }

    // Scalar tail for whatever didn't divide evenly into 16-pixel chunks.
    // Deliberately re-derives the same formula rather than calling
    // blend_over_scalar, to keep this file self-contained.
    for (; i < count; ++i) {
        const size_t base = i * 4;
        const unsigned a = src[base + 3];
        const unsigned inv_a = 255u - a;
        for (int c = 0; c < 3; ++c) {
            const unsigned s = src[base + c];
            const unsigned d = dst[base + c];
            const unsigned sum = s * a + d * inv_a;
            const unsigned t = sum + 128;
            out[base + c] = static_cast<uint8_t>((t + (t >> 8)) >> 8);
        }
        out[base + 3] = 255;
    }
}

}  // namespace renderer

#endif  // RENDERER_HAS_NEON
