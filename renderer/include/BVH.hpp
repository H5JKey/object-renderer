#pragma once
#include "scene.hpp"
#include <vector>
#include <limits>

class BVH {
public:
    struct Node {
        vec3 min, max;
        uint left, right;
        std::vector<uint> trianglesIndices;

        Node() : min(0,0,0), max(0,0,0), left(-1), right(-1), trianglesIndices(0) {
        }
    };

    BVH();
    void build(const Scene& scene);
private:
    void expandToFitTriangle(Node& node, vec3 v0, vec3 v1, vec3 v2);
    int split(int parentNodeIdx, const Scene& scene, int depth);
    std::vector<Node> nodes;
};