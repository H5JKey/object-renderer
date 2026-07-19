#pragma once
#include "scene.hpp"
#include <vector>
#include <limits>
#include "BVH.hpp"

class BVHBuilder {
public:
    BVHBuilder(int depthLimit, int trianglesLimit);
    virtual BVH build(const Scene& scene) const = 0;
    void setDepthLimit(int limit);
    void setTrianglesLimit(int limit);
    virtual ~BVHBuilder() = default;
protected:
    int trianglesLimit;
    int depthLimit;
};

class MedianBuilder : public BVHBuilder {
public:
    MedianBuilder(int depthLimit, int trianglesLimit);
    BVH build(const Scene& scene) const override;
private:
    int split(BVH& bvh, int parentNodeIdx, const Scene& scene, std::vector<int>::iterator begin, std::vector<int>::iterator end, int depth) const;
};