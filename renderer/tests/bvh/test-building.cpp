#include <gtest/gtest.h>

#include <set>
#include <stack>
#include <stdexcept>

#include "bvh-builder.hpp"
#include "scene.hpp"

class BVHBuildingTest : public ::testing::Test {
   protected:
    void SetUp() override {
        vertices = {
            vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 1),
            vec3(1, 0, 0), vec3(1, 0, 1), vec3(1, 1, 0), vec3(1, 1, 1),
        };
        vertexIndices = {0, 1, 2, 3, 2, 1, 3, 1, 5, 7, 3, 5, 4, 7, 5, 4, 6, 7,
                         6, 0, 2, 6, 4, 0, 6, 2, 3, 7, 6, 3, 4, 1, 0, 1, 4, 5};
    }
    std::vector<vec3> vertices;
    std::vector<int> vertexIndices;

   public:
    void BVHTraversal(const BVH& bvh, int depthLimit, int triangleLimit, int& leaf) {
        auto nodes = bvh.getNodes();
        auto triangles = bvh.getTriangles();
        int total_triangles = 0;
        leaf = 0;
        std::stack<std::pair<int, int>> stack;
        std::set<int> visitedNodes;
        stack.push({0, 0});
        while (!stack.empty()) {
            auto [idx, depth] = stack.top();
            if (visitedNodes.find(idx) != visitedNodes.end()) {
                EXPECT_TRUE(false);
            }
            visitedNodes.insert(idx);
            stack.pop();
            if (depthLimit > 0) EXPECT_LE(depth, depthLimit);
            BVH::Node node = nodes[idx];
            if (node.left == -1 && node.right == -1) {
                if (triangleLimit > 0) EXPECT_LE(node.count, triangleLimit);
                total_triangles += node.count;
                leaf++;
            } else if (node.left > 0 && node.right > 0) {
                EXPECT_EQ(node.count, 0);
                EXPECT_EQ(node.start, 0);
                stack.push({node.left, depth + 1});
                stack.push({node.right, depth + 1});
            } else {
                EXPECT_TRUE(false);
            }
        }
        EXPECT_EQ(visitedNodes.size(), nodes.size());
        EXPECT_EQ(total_triangles, triangles.size());
    }
};

TEST_F(BVHBuildingTest, BuildingSingleTrianglePerNode) {
    MedianBuilder builder(-1, 1);

    BVH bvh = builder.build(vertices, vertexIndices);
    EXPECT_EQ(bvh.getTriangles().size(), vertexIndices.size() / 3);
    int leaf;
    BVHTraversal(bvh, -1, 1, leaf);
    EXPECT_EQ(leaf, bvh.getTriangles().size());
}

TEST_F(BVHBuildingTest, BuildingDepthLimitOne) {
    MedianBuilder builder(1, -1);

    BVH bvh = builder.build(vertices, vertexIndices);
    int leaf;
    BVHTraversal(bvh, 1, -1, leaf);
}

TEST_F(BVHBuildingTest, EmptyScene) {
    std::vector<vec3> vertices = {};
    std::vector<int> vertexIndices = {};
    MedianBuilder builder(-1, -1);

    BVH bvh = builder.build(vertices, vertexIndices);
    EXPECT_EQ(bvh.getNodes().size(), 0);
    EXPECT_EQ(bvh.getTriangles().size(), 0);
}

TEST_F(BVHBuildingTest, InvalidVertexIndicies) {
    std::vector<int> vertexIndices;
    std::vector<vec3> vertices = {vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0)};
    MedianBuilder builder(-1, -1);

    vertexIndices = {1, 2};
    /* Vertex indicies array size must be multiple of 3 */
    EXPECT_THROW(builder.build(vertices, vertexIndices), std::runtime_error);

    vertexIndices = {0, 1, -1};
    /* Vertex index is out of range */
    EXPECT_THROW(builder.build(vertices, vertexIndices), std::runtime_error);

    vertexIndices = {1, 0, 6};
    /* Vertex index is out of range */
    EXPECT_THROW(builder.build(vertices, vertexIndices), std::runtime_error);
}