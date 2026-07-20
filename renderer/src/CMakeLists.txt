add_library(renderer_core STATIC
    scene.cpp
    scene-loader.cpp
    render-engine.cpp
    render-target.cpp
    target-manager.cpp
    denoiser.cpp
    bvh.cpp
    bvh-builder.cpp
    utils.cpp
)

target_link_libraries(renderer_core PUBLIC
    renderer_deps
)

target_include_directories(renderer_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(renderer
    main.cpp
)

target_link_libraries(renderer PRIVATE
    renderer_core
)

target_compile_options(renderer PRIVATE 
    -O3 
    -ffast-math 
    -march=native
    -fno-trapping-math
    -fno-signed-zeros
    -fno-math-errno
    -fassociative-math
    -frename-registers
    -fomit-frame-pointer
    -ftree-vectorize 
    -funroll-loops
    -flto=auto
)

add_custom_command(
    TARGET renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
    COMMENT "Copying shaders to build directory"
)