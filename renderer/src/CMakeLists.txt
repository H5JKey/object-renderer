add_subdirectory(core)
if (BUILD_CLI)
    add_subdirectory(cli)
endif()

if (BUILD_MICROSERVICE)
    add_subdirectory(web)
endif()