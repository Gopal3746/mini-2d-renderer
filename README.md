cat > README.md << 'EOF'
# mini-renderer

A small 2D software renderer built from scratch in C++20 — no Skia, no
external rasterization libraries. Goal: understand what a graphics
library like Skia actually does under the hood.

Pipeline: Scene (rects, lines, circles, Bézier paths) → affine
transforms (translate/rotate/scale via a 3x3 matrix) → rasterizer
(coverage, clipping, anti-aliasing, alpha blending) → RGBA
framebuffer → PNG.

## Build

    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    ./build/mini_renderer
EOF
