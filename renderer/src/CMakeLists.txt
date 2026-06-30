add_executable(renderer
    main.cpp
    scene.cpp
    scene-loader.cpp
    render-engine.cpp
)

target_link_libraries(renderer PRIVATE
    renderer_deps
)

target_include_directories(renderer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)