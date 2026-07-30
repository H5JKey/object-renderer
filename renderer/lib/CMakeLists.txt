add_subdirectory(core)
if (BUILD_MICROSERVICE)
    add_subdirectory(web)
endif()