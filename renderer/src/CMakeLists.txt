add_executable(renderer
    main.cpp
    scene.cpp
    scene-loader.cpp
)

target_link_libraries(renderer PRIVATE nlohmann_json::nlohmann_json)