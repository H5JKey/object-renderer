#pragma once

#include <glad/gl.h>

#include "render-target.hpp"
#include "scene.hpp"

class Engine {
public:
    Engine();

    void renderFrame(RenderTarget& target, const Scene& scene) const;

};