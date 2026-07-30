add_subdirectory(core)
if (BUILD_CLI)
    add_subdirectory(cli)
endif()

if (BUILD_WORKER)
    add_subdirectory(worker)
endif()