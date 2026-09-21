// Performance experiment: scalar alpha blending vs ARM NEON SIMD
// blending. Blends a 1920x1080 RGBA8 image over another, 100 times, and
// reports timing for each implementation available in this build.
//
// blend_over_scalar and blend_over_neon (renderer/blend.hpp) implement
// the exact same integer approximation of source-over onto an opaque
// destination, so any timing difference here reflects genuine SIMD
// parallelism, not a difference in what's being computed -- and this
// program checks that claim itself, by comparing their outputs for
// exact equality before trusting the timing numbers at all.

#include <chrono>
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>

#include "renderer/blend.hpp"

namespace {

constexpr int kWidth = 1920;
constexpr int kHeight = 1080;
constexpr size_t kPixelCount = static_cast<size_t>(kWidth) * kHeight;
constexpr int kIterations = 100;

std::vector<uint8_t> make_random_buffer(size_t pixel_count, unsigned seed) {
    std::vector<uint8_t> buffer(pixel_count * 4);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, 255);
    for (auto& byte : buffer) {
        byte = static_cast<uint8_t>(dist(rng));
    }
    return buffer;
}

template <typename BlendFn>
double time_blend_ms(BlendFn&& blend_fn, const std::vector<uint8_t>& src,
                      const std::vector<uint8_t>& dst, std::vector<uint8_t>& out,
                      int iterations) {
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) {
        blend_fn(src.data(), dst.data(), out.data(), kPixelCount);
    }
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

}  // namespace

int main() {
    std::printf("Blending %dx%d (%zu pixels) x %d iterations\n\n", kWidth, kHeight, kPixelCount,
                kIterations);

    const std::vector<uint8_t> src = make_random_buffer(kPixelCount, 1);
    const std::vector<uint8_t> dst = make_random_buffer(kPixelCount, 2);
    std::vector<uint8_t> out_scalar(kPixelCount * 4);

    const double scalar_ms =
        time_blend_ms(renderer::blend_over_scalar, src, dst, out_scalar, kIterations);
    std::printf("Scalar: %8.2f ms total, %.4f ms/iteration\n", scalar_ms,
                scalar_ms / kIterations);

#if RENDERER_HAS_NEON
    std::vector<uint8_t> out_neon(kPixelCount * 4);
    const double neon_ms =
        time_blend_ms(renderer::blend_over_neon, src, dst, out_neon, kIterations);
    std::printf("NEON:   %8.2f ms total, %.4f ms/iteration\n", neon_ms, neon_ms / kIterations);
    std::printf("\nSpeedup: %.2fx\n", scalar_ms / neon_ms);

    if (std::memcmp(out_scalar.data(), out_neon.data(), out_scalar.size()) == 0) {
        std::printf("Correctness: NEON output matches scalar exactly.\n");
    } else {
        std::fprintf(stderr, "Correctness: MISMATCH between scalar and NEON output!\n");
        return 1;
    }
#else
    std::printf("\nNEON not available in this build (RENDERER_HAS_NEON=0).\n");
    std::printf("Build on an ARM64 target (e.g. Apple Silicon) to compare.\n");
#endif

    return 0;
}
