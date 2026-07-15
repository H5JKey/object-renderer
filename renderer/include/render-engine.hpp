#pragma once

#include <glad/gl.h>

#include "render-target.hpp"
#include "scene.hpp"

class ContextGuard {

};

class RenderEngine {
private:
    GLuint pathTracingProgram;
    GLuint postProcessingProgram;
    GLuint gbufferProgram;
public:
    RenderEngine();
    GLuint compileShader(const std::string& source);
    void renderFrame(RenderTarget& target, const Scene& scene) const;
    std::string readFromFile(const std::string& path) const;
};