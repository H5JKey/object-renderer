add_executable(renderer_worker
    main.cpp
    kafka-consumer.cpp
    s3-client.cpp
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