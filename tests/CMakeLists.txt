add_executable(renderer_tests
  test_main.cpp
  test_vec2.cpp
  test_color.cpp
  test_matrix3x3.cpp
  test_framebuffer.cpp
  test_rasterizer.cpp
  test_bezier.cpp
)

# renderer_core's PUBLIC include dirs (include/, external/stb) propagate
# automatically through this link -- only the test-only doctest header
# needs adding here.
target_link_libraries(renderer_tests PRIVATE renderer_core)
target_include_directories(renderer_tests SYSTEM PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest
)

include(CTest)
add_test(NAME renderer_tests COMMAND renderer_tests)
