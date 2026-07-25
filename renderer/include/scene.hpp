#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>
#include <vector>

class SceneLoader;

class Scene {
   public:
    struct Mesh {
        struct Primitive {
            int startVertexIndex;
            int vertexIndicesCount;
            int materialId;
        };
        std::vector<int> vertexIndices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;

        std::vector<Primitive> primitives;
    };

    struct TextureData {
        std::vector<uint8_t> pixels;
        int width = 0;
        int height = 0;
        int id = -1;
    };

    struct Material {
        glm::vec3 albedo;
        glm::vec3 emission;
        float metalness;
        float roughness;
        float transmission;
        float ior;
        int albedoTextureID = -1;

        Material()
            : albedo(glm::vec3(0.3)),
              emission(glm::vec3(0.0)),
              metalness(0.0),
              roughness(0.5),
              transmission(0.0),
              ior(1.5),
              albedoTextureID(-1) {}
    };

    struct Camera {
        glm::vec3 origin;
        glm::vec3 lookAt;
        float fov;
    };

   private:
    friend class SceneLoader;

    Camera camera;
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    glm::vec3 backgroundColor;
    std::vector<TextureData> textures;

   public:
    Scene();
    Camera getCamera() const noexcept;
    glm::vec3 getBackgroundColor() const noexcept;
    const std::vector<Mesh>& getMeshes() const noexcept;
    const std::vector<Material>& getMaterials() const noexcept;
    const std::vector<TextureData>& getTexturesData() const noexcept;
};