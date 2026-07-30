add_executable(renderer_web
    main.cpp
)

target_link_libraries(renderer_web PRIVATE
    renderer_core
    renderer_web_deps
)

target_include_directories(renderer_web PRIVATE
     ${CMAKE_SOURCE_DIR}/include/core
)

target_compile_options(renderer_web PRIVATE 
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