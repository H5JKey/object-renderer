#pragma once

#include <glad/gl.h>

#include "render-target.hpp"
#include "scene.hpp"

class ContextGuard {

};

class RenderEngine {
private:
    GLuint program;
public:
    RenderEngine();
    void compileShaders();
    void renderFrame(RenderTarget& target, const Scene& scene) const;

};