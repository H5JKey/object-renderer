add_subdirectory(core)
if (BUILD_WORKER)
    add_subdirectory(worker)
endif()