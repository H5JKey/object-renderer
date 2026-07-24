#pragma once
#include <string>

#include "scene.hpp"

class SceneLoader {
   public:
    virtual Scene loadGltf(const std::string& path);

    ~SceneLoader() = default;
};