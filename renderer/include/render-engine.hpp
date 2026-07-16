#pragma once

#include <glad/gl.h>

#include "render-target.hpp"
#include "scene.hpp"
#include "denoiser.hpp"

class RenderEngine {
    GLuint pathTracingProgram;
    GLuint postProcessingProgram;
    GLuint gbufferProgram;

    GLuint vertexSSBO;
    GLuint vertexIndexSSBO;
    GLuint materialSSBO;
    GLuint materialIndexSSBO;
public:
    RenderEngine();
    void renderFrame(RenderTarget& target, const Scene& scene);
    ~RenderEngine();
private:
    void pathTracing(RenderTarget& target, const Scene& scene);
    void fillGbuffer(RenderTarget& target, const Scene& scene);
    void postProcess(RenderTarget& target) const;
    void loadSceneToGPU(const Scene& scene);
    GLuint compileShader(const std::string& source);
};