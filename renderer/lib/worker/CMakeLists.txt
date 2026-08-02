add_subdirectory(cppkafka)
add_subdirectory(json)

add_library(renderer_worker_deps INTERFACE)

target_link_libraries(renderer_worker_deps INTERFACE 
    cppkafka
    nlohmann_json::nlohmann_json
)

target_include_directories(renderer_worker_deps INTERFACE
    ${cppkafka_SOURCE_DIR}/include
)