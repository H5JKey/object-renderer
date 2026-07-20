#pragma once
#include <limits>
#include <vector>

#include "BVH.hpp"
#include "scene.hpp"

class BVHBuilder {
   public:
    BVHBuilder(int depthLimit, int trianglesLimit);
    virtual BVH build(const std::vector<vec3>& vertices, const std::vector<int>& vertexIndices) const = 0;
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
    BVH build(const std::vector<vec3>& vertices, const std::vector<int>& vertexIndices) const override;

   private:
    int split(BVH& bvh, int parentNodeIdx, const std::vector<vec3>& vertices, const std::vector<int>& vertexIndices,
              std::vector<int>::iterator begin, std::vector<int>::iterator end, int depth) const;
};