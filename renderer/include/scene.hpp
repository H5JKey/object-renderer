#pragma once
#include <glm/glm.hpp>
#include <vector>

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

class Scene {
   public:
    std::vector<int> vertexIndices;
    std::vector<glm::vec4> vertices;
    std::vector<Material> materials;
    std::vector<int> materialIndices;
    glm::vec4 origin;
    glm::vec4 lookAt;
    glm::vec4 backgroundColor;
    glm::vec4 sunColor;
    glm::vec4 sunDirection;

   public:
    Scene();
};