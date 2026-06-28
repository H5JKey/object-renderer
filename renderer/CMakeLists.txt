cmake_minimum_required(VERSION 3.28)
project(Renderer
    LANGUAGES CXX
    VERSION 1.0.0
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(libs)
target_include_directories(renderer PRIVATE include)