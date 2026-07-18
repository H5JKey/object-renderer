#include "BVH.hpp"

BVH::BVH() {
    nodes.reserve(128);
}

void BVH::expandToFitTriangle(Node& node, vec3 v0, vec3 v1, vec3 v2) {
    node.max.x = std::max(node.max.x, std::max(v0.x, std::max(v1.x, v2.x)));
    node.max.y = std::max(node.max.y, std::max(v0.y, std::max(v1.y, v2.y)));
    node.max.z = std::max(node.max.z, std::max(v0.z, std::max(v1.z, v2.z)));

    node.min.x = std::min(node.min.x, std::min(v0.x, std::min(v1.x, v2.x)));
    node.min.y = std::min(node.min.y, std::min(v0.y, std::min(v1.y, v2.y)));
    node.min.z = std::min(node.min.z, std::min(v0.z, std::min(v1.z, v2.z)));
}

int BVH::split(int parentNodeIdx, const Scene& scene, int depth) {
    Node& node = nodes[parentNodeIdx];
    if (node.trianglesIndices.size() <= 4 || depth >= 8) return parentNodeIdx;
    Node leftNode, rightNode;
    float offsetX = node.max.x - node.min.x;
    float middleX = (node.max.x + node.min.x) / 2;

    float offsetY = node.max.y - node.min.y;
    float middleY = (node.max.y + node.min.y) / 2;

    float offsetZ = node.max.z - node.min.z;
    float middleZ = (node.max.z + node.min.z) / 2;

    leftNode.trianglesIndices.reserve(node.trianglesIndices.size() / 2);
    rightNode.trianglesIndices.reserve(node.trianglesIndices.size() / 2);
    int axis;
    if (offsetX == std::max(offsetX, std::max(offsetY, offsetZ))) { 
        leftNode.min = node.min;
        leftNode.max.x = middleX;
        leftNode.max.y = node.max.y;
        leftNode.max.z = node.max.z;

        rightNode.max = node.max;
        rightNode.min.x = middleX;
        rightNode.min.y = node.min.y;
        rightNode.min.z = node.min.z;
        axis = 0;
    }
    else if (offsetY == std::max(offsetX, std::max(offsetY, offsetZ))) {
        leftNode.min = node.min;
        leftNode.max.y = middleY;
        leftNode.max.z = node.max.z;
        leftNode.max.x = node.max.x;

        rightNode.max = node.max;
        rightNode.min.y = middleY;
        rightNode.min.z = node.min.z;
        rightNode.min.x = node.min.x;
        axis = 1;
    }
    else {
        leftNode.min = node.min;
        leftNode.max.z = middleZ;
        leftNode.max.x = node.max.x;
        leftNode.max.y = node.max.y;

        rightNode.max = node.max;
        rightNode.min.z = middleZ;
        rightNode.min.y = node.min.y;
        rightNode.min.x = node.min.x;
        axis = 2;
    }
    for(int i=0; i < node.trianglesIndices.size(); ++i) {
        auto v0 = scene.vertices[scene.vertexIndices[3*node.trianglesIndices[i]]];
        auto v1 = scene.vertices[scene.vertexIndices[3*node.trianglesIndices[i]+1]];
        auto v2 = scene.vertices[scene.vertexIndices[3*node.trianglesIndices[i]+2]];

        vec3 center((v0.x+v1.x+v2.x) / 3, (v0.y+v1.y+v2.y) / 3, (v0.z+v1.z+v2.z) / 3);
        if (axis == 0 && center.x <= middleX ||
            axis == 1 && center.y <= middleY ||
            axis == 2 && center.z <= middleZ
        ) {
            expandToFitTriangle(leftNode, v0, v1, v2);
            leftNode.trianglesIndices.push_back(node.trianglesIndices[i]);
        }
        else {
            expandToFitTriangle(rightNode, v0, v1, v2);
            rightNode.trianglesIndices.push_back(node.trianglesIndices[i]);
        }
    }
    int currentIdx = nodes.size() - 1;
    if (leftNode.trianglesIndices.size() == 0 || rightNode.trianglesIndices.size() == 0) {
        return currentIdx;
    }

    nodes.push_back(leftNode);
    node.left = split(nodes.size()-1, scene, depth+1);

    nodes.push_back(rightNode);
    node.right = split(nodes.size()-1, scene, depth+1);

    node.trianglesIndices.clear();
    return currentIdx;
    
}

void BVH::build(const Scene& scene) {
    const auto INFINITY = std::numeric_limits<float>::infinity();
    vec3 max(-INFINITY,-INFINITY,-INFINITY), min(INFINITY,INFINITY,INFINITY);

    Node node;
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
        node.trianglesIndices.push_back(triangleIdx);
    }
    node.max = max;
    node.min = min;
    nodes.push_back(node);
    split(0, scene, 0);
}