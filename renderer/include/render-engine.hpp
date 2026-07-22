#pragma once
#include <glad/gl.h>

#include <random>

#include "bvh.hpp"
#include "denoiser.hpp"
#include "glm/ext/vector_float4.hpp"
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
    void renderFrame(RenderTarget& target, const MeshData& meshData, const Camera& camera,
                     const glm::vec4& backgroundColor, const BVH& bvh);
    ~RenderEngine();

   private:
    void pathTracing(RenderTarget& target, const MeshData& meshData, const Camera& camera,
                     const glm::vec4& backgroundColor);
    void fillGbuffer(RenderTarget& target, const MeshData& meshData, const Camera& camera);
    void postProcess(RenderTarget& target) const;
    void loadDataToGPU(const MeshData& meshData, const BVH& bvh);
    GLuint compileShader(const std::string& source);
};