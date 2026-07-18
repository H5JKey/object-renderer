#include "BVH.hpp"
#include <algorithm>
#include <stdexcept>

BVH::BVH(int depthLimit, int trianglesLimit) : depth(0) {
    nodes.reserve(128);
    setDepthLimit(depthLimit);
    setTrianglesLimit(trianglesLimit);
}

void BVH::expandToFitTriangle(Node& node, vec3 v0, vec3 v1, vec3 v2) {
    node.max.x = std::max(node.max.x, std::max(v0.x, std::max(v1.x, v2.x)));
    node.max.y = std::max(node.max.y, std::max(v0.y, std::max(v1.y, v2.y)));
    node.max.z = std::max(node.max.z, std::max(v0.z, std::max(v1.z, v2.z)));

    node.min.x = std::min(node.min.x, std::min(v0.x, std::min(v1.x, v2.x)));
    node.min.y = std::min(node.min.y, std::min(v0.y, std::min(v1.y, v2.y)));
    node.min.z = std::min(node.min.z, std::min(v0.z, std::min(v1.z, v2.z)));
}

int BVH::split(int parentNodeIdx, const Scene& scene, std::vector<int>::iterator begin, std::vector<int>::iterator end, int depth) {
    Node& node = nodes[parentNodeIdx];
    if (end - begin <= trianglesLimit || depth >= depthLimit) {
        node.start = begin - trianglesIndices.begin();
        node.count = end-begin;
        node.left = -1;
        node.right = -1;
        this->depth = std::max(depth, this->depth);
        return parentNodeIdx;
    }
    Node leftNode, rightNode;
    float offsetX = node.max.x - node.min.x;
    float offsetY = node.max.y - node.min.y;
    float offsetZ = node.max.z - node.min.z;

    int axis;
    if (offsetX >= offsetY && offsetX >= offsetZ) 
        axis = 0;
    else if (offsetY >= offsetX && offsetY >= offsetZ)
        axis = 1;
    else 
        axis = 2;
    std::nth_element(begin, begin + (end - begin) / 2, end, [&](int i, int j) {
        auto v0_i = scene.vertices[scene.vertexIndices[3*i]];
        auto v1_i = scene.vertices[scene.vertexIndices[3*i+1]];
        auto v2_i = scene.vertices[scene.vertexIndices[3*i+2]];

        auto v0_j = scene.vertices[scene.vertexIndices[3*j]];
        auto v1_j = scene.vertices[scene.vertexIndices[3*j+1]];
        auto v2_j = scene.vertices[scene.vertexIndices[3*j+2]];
        if (axis == 0) return v0_i.x+v1_i.x+v2_i.x <= v0_j.x+v1_j.x+v2_j.x;
        else if (axis == 1) return v0_i.y+v1_i.y+v2_i.y <= v0_j.y+v1_j.y+v2_j.y;
        else return v0_i.z+v1_i.z+v2_i.z <= v0_j.z+v1_j.z+v2_j.z;
    });
    auto median = begin + (end - begin) / 2;
    int medianIdx = *median;
    auto v0 = scene.vertices[scene.vertexIndices[3 * medianIdx]];
    auto v1 = scene.vertices[scene.vertexIndices[3 * medianIdx + 1]];
    auto v2 = scene.vertices[scene.vertexIndices[3 * medianIdx + 2]];

    float splitPos;
    if (axis == 0) splitPos = (v0.x + v1.x + v2.x) / 3.0f;
    else if (axis == 1) splitPos = (v0.y + v1.y + v2.y) / 3.0f;
    else splitPos = (v0.z + v1.z + v2.z) / 3.0f;
    if (axis == 0) {
        leftNode.min = node.min;
        leftNode.max.x = splitPos;
        leftNode.max.y = node.max.y;
        leftNode.max.z = node.max.z;

        rightNode.max = node.max;
        rightNode.min.x = splitPos;
        rightNode.min.y = node.min.y;
        rightNode.min.z = node.min.z;
    }
    else if (axis == 1) {
        leftNode.min = node.min;
        leftNode.max.y = splitPos;
        leftNode.max.z = node.max.z;
        leftNode.max.x = node.max.x;

        rightNode.max = node.max;
        rightNode.min.y = splitPos;
        rightNode.min.z = node.min.z;
        rightNode.min.x = node.min.x;
    }
    else {
        leftNode.min = node.min;
        leftNode.max.z = splitPos;
        leftNode.max.x = node.max.x;
        leftNode.max.y = node.max.y;

        rightNode.max = node.max;
        rightNode.min.z = splitPos;
        rightNode.min.y = node.min.y;
        rightNode.min.x = node.min.x;
    }
    for (auto it = begin; it != median; ++it) {
        int triangleIdx = *it;
        auto v0 = scene.vertices[scene.vertexIndices[3*triangleIdx]];
        auto v1 = scene.vertices[scene.vertexIndices[3*triangleIdx+1]];
        auto v2 = scene.vertices[scene.vertexIndices[3*triangleIdx+2]];
        expandToFitTriangle(leftNode, v0, v1, v2);
    }

    for (auto it = median; it != end; ++it) {
        int triangleIdx = *it;
        auto v0 = scene.vertices[scene.vertexIndices[3*triangleIdx]];
        auto v1 = scene.vertices[scene.vertexIndices[3*triangleIdx+1]];
        auto v2 = scene.vertices[scene.vertexIndices[3*triangleIdx+2]];
        expandToFitTriangle(rightNode, v0, v1, v2);
    }
    int currentIdx = nodes.size() - 1;
    if (begin == median || median == end) {
        return parentNodeIdx;
    }

    nodes.push_back(leftNode);
    nodes[parentNodeIdx].left = split(nodes.size()-1, scene, begin, median, depth+1);

    nodes.push_back(rightNode);
    nodes[parentNodeIdx].right = split(nodes.size()-1, scene, median, end, depth+1);

    nodes[parentNodeIdx].start = 0;
    nodes[parentNodeIdx].count = 0;
    return currentIdx;
    
}

void BVH::build(const Scene& scene) {
    trianglesIndices.clear();

    const auto INFINITY = std::numeric_limits<float>::infinity();
    vec3 max(-INFINITY,-INFINITY,-INFINITY), min(INFINITY,INFINITY,INFINITY);

    Node node;
    trianglesIndices.reserve(scene.vertexIndices.size() / 3);
    for(int triangleIdx=0; triangleIdx < scene.vertexIndices.size() / 3; ++triangleIdx) {
        auto v1 = scene.vertices[scene.vertexIndices[3*triangleIdx]];
        auto v2 = scene.vertices[scene.vertexIndices[3*triangleIdx+1]];
        auto v3 = scene.vertices[scene.vertexIndices[3*triangleIdx+2]];

        min.x = std::min(min.x, std::min(v1.x, std::min(v2.x, v3.x)));
        min.y = std::min(min.y, std::min(v1.y, std::min(v2.y, v3.y)));
        min.z = std::min(min.z, std::min(v1.z, std::min(v2.z, v3.z)));

        max.x = std::max(max.x, std::max(v1.x, std::max(v2.x, v3.x)));
        max.y = std::max(max.y, std::max(v1.y, std::max(v2.y, v3.y)));
        max.z = std::max(max.z, std::max(v1.z, std::max(v2.z, v3.z)));
        trianglesIndices.push_back(triangleIdx);
    }
    node.max = max;
    node.min = min;
    nodes.push_back(node);
    split(0, scene, trianglesIndices.begin(), trianglesIndices.end(), 0);
}

const std::vector<BVH::Node>& BVH::getNodes() const noexcept {
    return nodes;
}

const std::vector<int>& BVH::getTriangles() const noexcept {
    return trianglesIndices;
}

int BVH::getDepth() const noexcept {
    return depth;
}

void BVH::setDepthLimit(int limit) {
    if (limit < 0) {
        throw std::runtime_error("Depth limit should be greater than zero");
    }
    depthLimit = limit;
}

void BVH::setTrianglesLimit(int limit) {
    if (limit < 0) {
        throw std::runtime_error("Triangles limit should be greater than zero");
    }
    trianglesLimit = limit;
}