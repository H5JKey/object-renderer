#pragma once
#include <limits>
#include <vector>

#include "scene.hpp"

class BVH {
    friend class MedianBuilder;
    BVH();

   public:
    struct Node {
        vec3 min, max;
        int left, right;
        int start;
        int count;
        Node() : min(0, 0, 0), max(0, 0, 0), left(-1), right(-1) {}
        void expandToFitTriangle(vec3 v0, vec3 v1, vec3 v2);
    };
    const std::vector<Node>& getNodes() const noexcept;
    const std::vector<int>& getTriangles() const noexcept;
    int getDepth() const noexcept;

   private:
    std::vector<Node> nodes;
    std::vector<int> trianglesIndices;
    int depth;
};