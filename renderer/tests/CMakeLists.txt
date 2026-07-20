include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test
    test-rgb-rgba.cpp
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
    GTest::gtest
)

include(GoogleTest)
add_test(NAME test COMMAND test)
gtest_discover_tests(test)