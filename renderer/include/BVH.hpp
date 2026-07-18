#pragma once
#include "scene.hpp"
#include <vector>
#include <limits>

class BVH {
public:
    struct Node {
        vec3 min, max;
        int left, right;
        int start;
        int count;

        Node() : min(0,0,0), max(0,0,0), left(-1), right(-1) {
        }
    };

    BVH();
    void build(const Scene& scene);
    const std::vector<Node>& getNodes() const;
    const std::vector<int>& getTriangles() const;
private:
    void expandToFitTriangle(Node& node, vec3 v0, vec3 v1, vec3 v2);
    int split(int parentNodeIdx, const Scene& scene, std::vector<int>::iterator begin, std::vector<int>::iterator end, int depth);
    std::vector<Node> nodes;
    std::vector<int> trianglesIndices;
};