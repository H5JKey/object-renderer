#pragma once
#include <vector>

struct vec3 {
    vec3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    float x, y, z;
    float padding;
};

class Scene {
public:
    std::vector<int> indices;
    std::vector<vec3> vertices;

    vec3 origin;
    vec3 lookAt;
    vec3 backgroundColor;
    vec3 sunColor;
    vec3 sunDirection;
public:
    Scene();
};