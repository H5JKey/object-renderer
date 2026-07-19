include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test
    test.cpp
)

target_link_libraries(test PRIVATE
    renderer_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test)