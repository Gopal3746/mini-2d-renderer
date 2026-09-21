cmake_minimum_required(VERSION 3.16)
project(mini_renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra)

# Catch rasterizer bugs (off-by-one scanlines, OOB writes) the moment they
# happen instead of as a mysterious artifact in output.png. Only in Debug
# builds -- sanitizers have real runtime cost, so Release stays clean for
# the perf benchmark.
option(RENDERER_ENABLE_SANITIZERS "Build with ASan/UBSan in Debug builds" ON)
if(RENDERER_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# ---- NEON detection ----
# Apple Silicon reports "arm64"; 64-bit ARM Linux (Raspberry Pi 4/5, some
# GitHub-hosted runners) reports "aarch64". 32-bit ARM is out of scope --
# NEON isn't guaranteed there the way it is on every 64-bit ARM chip.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
  set(RENDERER_HAS_NEON_VALUE 1)
else()
  set(RENDERER_HAS_NEON_VALUE 0)
endif()

# ---- core library (everything except main.cpp / tests) ----
add_library(renderer_core STATIC
  src/framebuffer.cpp
  src/rasterizer.cpp
  src/bezier.cpp
  src/blend.cpp
)
if(RENDERER_HAS_NEON_VALUE)
  target_sources(renderer_core PRIVATE src/blend_neon.cpp)
endif()
# PUBLIC so every translation unit that links renderer_core (main.cpp,
# tests, the benchmark) sees the same RENDERER_HAS_NEON value that
# decided whether blend_neon.cpp was even compiled -- keeping the
# header's #if RENDERER_HAS_NEON guard consistent with what actually
# got built, everywhere, automatically.
target_compile_definitions(renderer_core PUBLIC RENDERER_HAS_NEON=${RENDERER_HAS_NEON_VALUE})

target_include_directories(renderer_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(renderer_core SYSTEM PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
)

# ---- demo executable ----
add_executable(mini_renderer src/main.cpp)
target_link_libraries(mini_renderer PRIVATE renderer_core)

# ---- tests ----
enable_testing()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
  add_subdirectory(tests)
endif()

# ---- benchmarks ----
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/CMakeLists.txt)
  add_subdirectory(benchmarks)
endif()
