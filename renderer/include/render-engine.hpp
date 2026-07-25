#pragma once
#include <glad/gl.h>

#include <map>
#include <random>

#include "bvh-builder.hpp"
#include "bvh.hpp"
#include "denoiser.hpp"
#include "glm/ext/vector_float4.hpp"
#include "render-target.hpp"
#include "scene.hpp"

class RenderEngine {
    struct GPUMaterial {
        glm::vec4 albedo;
        glm::vec4 emission;
        float metalness;
        float roughness;
        float transmission;
        float ior;
        int albedoTextureID;

       private:
        float padding[3];
    };

    struct GPUData {
        std::vector<glm::vec4> vertices;
        std::vector<int> vertexIndices;
        std::vector<GPUMaterial> materials;
        std::vector<int> materialIndices;
        std::vector<glm::vec4> texCoords;
        std::vector<GLuint> textures;
    };

    Denoiser denoiser;
    MedianBuilder bvhBuilder;
    GLuint pathTracingProgram;
    GLuint postProcessingProgram;
    GLuint gbufferProgram;

    GLuint vertexSSBO;
    GLuint vertexIndexSSBO;
    GLuint materialSSBO;
    GLuint materialIndexSSBO;
    GLuint bvhNodesSSBO;
    GLuint bvhTrianglesSSBO;
    GLuint texCoordSSBO;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<uint> uniformDistr;

    std::map<int, GLuint> loadedTextures;

   public:
    RenderEngine();
    void renderFrame(RenderTarget& target, const Scene& scene);
    ~RenderEngine();

   private:
    void pathTracing(RenderTarget& target, const GPUData& gpuData, const Scene::Camera& camera,
                     const glm::vec3 backgroundColor);
    void fillGbuffer(RenderTarget& target, const GPUData& gpuData, const Scene::Camera& camera);
    void postProcess(RenderTarget& target) const;
    void createGPUBuffers(const GPUData& gpuData, const BVH& bvh);
    GLuint compileShader(const std::string& source);
    GLuint loadTexture(const Scene::TextureData& texture);
    GPUData uploadSceneToGPU(const Scene& scene);
};