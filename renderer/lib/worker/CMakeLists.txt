add_subdirectory(cppkafka)

add_library(renderer_worker_deps INTERFACE)

target_link_libraries(renderer_worker_deps INTERFACE 
    cppkafka
)

target_include_directories(renderer_worker_deps INTERFACE
    ${cppkafka_SOURCE_DIR}/include
)