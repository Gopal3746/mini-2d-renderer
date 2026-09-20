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
# the eventual perf benchmark.
option(RENDERER_ENABLE_SANITIZERS "Build with ASan/UBSan in Debug builds" ON)
if(RENDERER_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# ---- core library (everything except main.cpp / tests) ----
add_library(renderer_core STATIC
  src/framebuffer.cpp
  src/rasterizer.cpp
)
target_include_directories(renderer_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(renderer_core SYSTEM PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
)

# ---- demo executable ----
add_executable(mini_renderer src/main.cpp)
target_link_libraries(mini_renderer PRIVATE renderer_core)

# ---- tests (added in a later milestone) ----
enable_testing()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
  add_subdirectory(tests)
endif()
