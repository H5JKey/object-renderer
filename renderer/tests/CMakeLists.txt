message(STATUS "Building tests")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test
    utils/test-rgb-rgba.cpp
    utils/test-imageReadWrite.cpp
    utils/test-readFromFile.cpp
    bvh/test-building.cpp
    bvh/test-intersections.cpp
    scene-loader/test-scene-loader.cpp
    renderer-pipeline/test-renderer-pipeline.cpp
    test-main.cpp
)

target_compile_options(test PRIVATE 
    -O0
    -g
    -fno-omit-frame-pointer
    -fno-inline
    -fno-optimize-sibling-calls 
)

target_link_libraries(test PRIVATE
    renderer_core
    GTest::gtest_main
)

add_custom_command(
    TARGET test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/tests/data
        ${CMAKE_BINARY_DIR}/tests/data
    COMMENT "Copying tests data to build directory"
)

include(GoogleTest)
add_test(NAME test COMMAND test)
gtest_discover_tests(test)