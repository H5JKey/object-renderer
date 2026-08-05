add_executable(renderer_worker
    main.cpp
    kafka-consumer.cpp
    kafka-producer.cpp
    s3-client.cpp
    dotenv.cpp
)

target_link_libraries(renderer_worker PRIVATE
    renderer_core
    renderer_worker_deps
)

target_include_directories(renderer_worker PRIVATE
     ${CMAKE_SOURCE_DIR}/include/worker
)

target_compile_options(renderer_worker PRIVATE 
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
    TARGET renderer_worker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/../.env
        ${CMAKE_BINARY_DIR}/.env
    COMMENT "Linking .env to build directory"
)