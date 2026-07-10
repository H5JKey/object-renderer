#pragma once
#include <vector>
#include <string>

struct vec3 {
    float x, y, z;
    float padding;
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Material {
    vec3 albedo;
    float metalness;
    float roughness;
    float padding[2];
};


class Scene {
public:
    std::vector<int> vertexIndices;
    std::vector<vec3> vertices;
    std::vector<Material> materials;
    std::vector<int> materialIndices;
    vec3 origin;
    vec3 lookAt;
    vec3 backgroundColor;
    vec3 sunColor;
    vec3 sunDirection;
public:
    Scene();
};