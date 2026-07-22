#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>
#include <vector>

struct Mesh {
    glm::vec4 position;
    glm::mat4 transform;

    std::vector<int> vertexIndices;
    std::vector<glm::vec4> vertices;
    int materialId;
};

struct Material {
    glm::vec4 albedo;
    glm::vec4 emission;
    float metalness;
    float roughness;
    float transmission;
    float ior;

    Material(glm::vec4 albedo, glm::vec4 emission, float metalness, float roughness, float transmission, float ior)
        : albedo(albedo),
          emission(emission),
          metalness(metalness),
          roughness(roughness),
          transmission(transmission),
          ior(ior) {}
};

struct Camera {
    glm::vec4 origin;
    glm::vec4 lookAt;
    float fov;
};

struct MeshData {
    std::vector<glm::vec4> vertices;
    std::vector<int> vertexIndices;
    std::vector<Material> materials;
    std::vector<int> materialIndices;
};

class Scene {
   private:
    Camera camera;
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    glm::vec4 backgroundColor;
    MeshData meshData;

   public:
    Scene();
    const MeshData& getMeshData() const noexcept;
    void buildMeshData();
    Camera getCamera() const noexcept;
    glm::vec4 getbackgroundColor() const noexcept;
};