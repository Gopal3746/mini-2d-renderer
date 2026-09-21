#include <cstdint>
#include <cstring>
#include <vector>

#include "doctest.h"
#include "renderer/blend.hpp"

TEST_CASE("blend_over_scalar: alpha=0 leaves destination unchanged") {
    const uint8_t src[4] = {200, 100, 50, 0};  // fully transparent source
    const uint8_t dst[4] = {10, 20, 30, 255};
    uint8_t out[4];
    renderer::blend_over_scalar(src, dst, out, 1);
    CHECK(out[0] == dst[0]);
    CHECK(out[1] == dst[1]);
    CHECK(out[2] == dst[2]);
    CHECK(out[3] == 255);
}

TEST_CASE("blend_over_scalar: alpha=255 copies source exactly") {
    const uint8_t src[4] = {200, 100, 50, 255};  // fully opaque source
    const uint8_t dst[4] = {10, 20, 30, 255};
    uint8_t out[4];
    renderer::blend_over_scalar(src, dst, out, 1);
    CHECK(out[0] == src[0]);
    CHECK(out[1] == src[1]);
    CHECK(out[2] == src[2]);
    CHECK(out[3] == 255);
}

TEST_CASE("blend_over_scalar: 50% alpha computes the exact div255 result") {
    const uint8_t src[4] = {200, 200, 200, 128};
    const uint8_t dst[4] = {0, 0, 0, 255};
    uint8_t out[4];
    renderer::blend_over_scalar(src, dst, out, 1);
    // sum = 200*128 + 0*127 = 25600; div255(25600) = 100 exactly (hand
    // -verified: 25600/255 = 100.39..., correctly rounds down to 100).
    CHECK(out[0] == 100);
}

#if RENDERER_HAS_NEON
TEST_CASE("blend_over_neon matches blend_over_scalar exactly, across many pixels") {
    // 1000 is deliberately not a multiple of 16 (NEON's per-iteration
    // width), specifically to exercise the scalar tail-handling path
    // inside blend_over_neon too, not just its main SIMD loop.
    constexpr size_t kCount = 1000;
    std::vector<uint8_t> src(kCount * 4);
    std::vector<uint8_t> dst(kCount * 4);
    for (size_t i = 0; i < src.size(); ++i) {
        src[i] = static_cast<uint8_t>((i * 37) % 256);
        dst[i] = static_cast<uint8_t>((i * 91) % 256);
    }

    std::vector<uint8_t> out_scalar(kCount * 4);
    std::vector<uint8_t> out_neon(kCount * 4);
    renderer::blend_over_scalar(src.data(), dst.data(), out_scalar.data(), kCount);
    renderer::blend_over_neon(src.data(), dst.data(), out_neon.data(), kCount);

    CHECK(std::memcmp(out_scalar.data(), out_neon.data(), out_scalar.size()) == 0);
}
#endif
