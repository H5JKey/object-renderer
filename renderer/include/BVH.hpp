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
        uint start;
        uint count;

        Node() : min(0,0,0), max(0,0,0), left(-1), right(-1) {
        }
    };

    BVH();
    void build(const Scene& scene);
    const std::vector<Node>& getNodes() const;
    const std::vector<uint>& getTriangles() const;
private:
    void expandToFitTriangle(Node& node, vec3 v0, vec3 v1, vec3 v2);
    uint split(uint parentNodeIdx, const Scene& scene, int depth);
    std::vector<Node> nodes;
    std::vector<uint> triangles;
};