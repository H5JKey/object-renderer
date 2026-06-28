#pragma once

#include <nlohmann/json.hpp>
#include "scene.hpp"

class SceneLoader {
public:
    virtual Scene load(const std::string& path) = 0;

    virtual ~SceneLoader() = default;
};

class JsonLoader : public SceneLoader{
public:
    Scene load(const std::string& path) override;
};