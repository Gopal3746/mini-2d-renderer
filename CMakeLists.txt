cmake_minimum_required(VERSION 3.16)
project(mini_renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra)

# ---- core library (everything except main.cpp / tests) ----
add_library(renderer_core STATIC
  src/framebuffer.cpp
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
