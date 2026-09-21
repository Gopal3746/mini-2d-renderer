#include "renderer/blend.hpp"

namespace renderer {

namespace {

// Exact integer division by 255 for x in [0, 255*255], via the classic
// "premultiplied alpha" trick (Blinn, "Three Wrongs Make a Right").
//
// A naive `x >> 8` is NOT the same thing -- it divides by 256, not 255,
// which is close but systematically biased: even x = 255*255 (the
// alpha=255 "should be exact" case) comes out wrong by 1 with a plain
// shift. This formula is genuinely exact, not just a close
// approximation, for every value in range -- verified by hand against
// several boundary cases while debugging the first version of this
// function, which used the naive shift and failed its own unit tests.
inline unsigned div255(unsigned x) {
    const unsigned t = x + 128;
    return (t + (t >> 8)) >> 8;
}

}  // namespace

void blend_over_scalar(const uint8_t* src, const uint8_t* dst, uint8_t* out, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        const size_t base = i * 4;
        const unsigned a = src[base + 3];
        const unsigned inv_a = 255u - a;

        for (int c = 0; c < 3; ++c) {
            const unsigned s = src[base + c];
            const unsigned d = dst[base + c];
            // s,d,a,inv_a are each <= 255; s*a and d*inv_a are each
            // <= 65025, and since a + inv_a == 255 always, their SUM is
            // also bounded by 255*255 = 65025 -- never overflows a
            // 16-bit value, well within div255's valid input range.
            const unsigned sum = s * a + d * inv_a;
            out[base + c] = static_cast<uint8_t>(div255(sum));
        }
        out[base + 3] = 255;  // dst assumed opaque; stays opaque
    }
}

}  // namespace renderer
