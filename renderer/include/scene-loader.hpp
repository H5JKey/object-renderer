#pragma once
#include <filesystem>
#include <string>

#include "scene.hpp"

class SceneLoader {
   public:
    virtual Scene loadGltf(const std::filesystem::path& path);

    ~SceneLoader() = default;
};