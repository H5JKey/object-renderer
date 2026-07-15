include(FetchContent)

add_subdirectory(json)
add_subdirectory(glad)
add_subdirectory(stb)

find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)
find_package(OpenImageDenoise REQUIRED)

add_library(renderer_deps INTERFACE)
target_include_directories(renderer_deps INTERFACE ${stb_SOURCE_DIR})
target_link_libraries(renderer_deps INTERFACE
    nlohmann_json::nlohmann_json
    OpenGL::OpenGL 
    OpenGL::EGL
    glad
)