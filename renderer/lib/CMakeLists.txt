include(FetchContent)

add_subdirectory(json)
add_subdirectory(glad)
add_subdirectory(glfw)

find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)

add_library(renderer_deps INTERFACE)
target_link_libraries(renderer_deps INTERFACE
    nlohmann_json::nlohmann_json
    OpenGL::OpenGL 
    OpenGL::EGL
    glfw
    glad
)