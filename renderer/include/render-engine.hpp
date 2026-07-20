#pragma once
#include <glad/gl.h>

#include <random>

#include "BVH.hpp"
#include "denoiser.hpp"
#include "render-target.hpp"
#include "scene.hpp"

class RenderEngine {
    Denoiser denoiser;
    GLuint pathTracingProgram;
    GLuint postProcessingProgram;
    GLuint gbufferProgram;

    GLuint vertexSSBO;
    GLuint vertexIndexSSBO;
    GLuint materialSSBO;
    GLuint materialIndexSSBO;
    GLuint bvhNodesSSBO;
    GLuint bvhTrianglesSSBO;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<uint> uniformDistr;

   public:
    RenderEngine();
    void renderFrame(RenderTarget& target, const Scene& scene, const BVH& bvh);
    ~RenderEngine();

   private:
    void pathTracing(RenderTarget& target, const Scene& scene);
    void fillGbuffer(RenderTarget& target, const Scene& scene);
    void postProcess(RenderTarget& target) const;
    void loadSceneToGPU(const Scene& scene, const BVH& bvh);
    GLuint compileShader(const std::string& source);
};