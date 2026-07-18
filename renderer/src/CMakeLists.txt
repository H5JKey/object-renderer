add_executable(renderer
    main.cpp
    scene.cpp
    scene-loader.cpp
    render-engine.cpp
    render-target.cpp
    target-manager.cpp
    denoiser.cpp
    BVH.cpp
)

target_link_libraries(renderer PRIVATE
    renderer_deps
)

target_include_directories(renderer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_custom_command(
    TARGET renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
    COMMENT "Copying shaders to build directory"
)