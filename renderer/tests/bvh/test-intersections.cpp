#include <gtest/gtest.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>

#include "bvh-builder.hpp"
#include "glm/trigonometric.hpp"

class BVHIntersectionsTest : public ::testing::Test {
   protected:
    void SetUp() override {
        vertices = {
            glm::vec4(0, 0, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 1, 1, 0),
            glm::vec4(1, 0, 0, 0), glm::vec4(1, 0, 1, 0), glm::vec4(1, 1, 0, 0), glm::vec4(1, 1, 1, 0),
        };
        vertexIndices = {0, 1, 2, 3, 2, 1, 3, 1, 5, 7, 3, 5, 4, 7, 5, 4, 6, 7,
                         6, 0, 2, 6, 4, 0, 6, 2, 3, 7, 6, 3, 4, 1, 0, 1, 4, 5};
    }
    std::vector<glm::vec4> vertices;
    std::vector<int> vertexIndices;

   private:
    float triangleIntersection(glm::vec3 ro, glm::vec3 rd, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) const {
        glm::vec3 e1 = v1 - v0;
        glm::vec3 e2 = v2 - v0;

        glm::vec3 pvec = cross(rd, e2);
        float det = dot(e1, pvec);

        if (abs(det) < 1e-8) return -1.0f;

        float inv_det = 1.0 / det;
        glm::vec3 tvec = ro - v0;

        float u = dot(tvec, pvec) * inv_det;
        if (u < 0.0 || u > 1.0) return -1.0f;

        glm::vec3 qvec = cross(tvec, e1);

        float v = dot(rd, qvec) * inv_det;
        if (v < 0.0 || u + v > 1.0) return -1.0f;

        float t = dot(e2, qvec) * inv_det;
        if (t < 1e-8) return -1.0f;

        return t;
    }
    float AABBIntersection(glm::vec3 origin, glm::vec3 direction, glm::vec3 boxMin, glm::vec3 boxMax) const {
        glm::vec3 invDir = 1.0f / direction;

        glm::vec3 t0 = (boxMin - origin) * invDir;
        glm::vec3 t1 = (boxMax - origin) * invDir;

        glm::vec3 tNear = glm::vec3(std::min(t0.x, t1.x), std::min(t0.y, t1.y), std::min(t0.z, t1.z));
        glm::vec3 tFar = glm::vec3(std::max(t0.x, t1.x), std::max(t0.y, t1.y), std::max(t0.z, t1.z));

        float tMin = std::max(std::max(tNear.x, tNear.y), std::max(tNear.z, 0.0f));
        float tMax = std::min(std::min(tFar.x, tFar.y), tFar.z);

        if (tMax >= 0.0 && tMin <= tMax + EPSILON)
            return tMin;
        else
            return -1.0;
    }

   public:
    void rotate(float angleX, float angleY, float angleZ) {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, angleX, glm::vec3(1, 0, 0));
        transform = glm::rotate(transform, angleY, glm::vec3(0, 1, 0));
        transform = glm::rotate(transform, angleZ, glm::vec3(0, 0, 1));

        for (auto& v : vertices) {
            v = transform * v;
        }
    }

    static constexpr float EPSILON = 1e-6f;
    float rayCastBruteForce(glm::vec3 origin, glm::vec3 direction) const {
        float closest = std::numeric_limits<float>::infinity();
        for (int triangle_idx = 0; triangle_idx < vertexIndices.size() / 3; triangle_idx++) {
            glm::vec3 v0 = vertices[vertexIndices[3 * triangle_idx]];
            glm::vec3 v1 = vertices[vertexIndices[3 * triangle_idx + 1]];
            glm::vec3 v2 = vertices[vertexIndices[3 * triangle_idx + 2]];
            float distance = triangleIntersection(origin, direction, v0, v1, v2);
            if (distance > 0.0f) closest = std::min(closest, distance);
        }
        return closest;
    }

    float rayCastBVH(glm::vec3 origin, glm::vec3 direction, const BVH& bvh) const {
        float closest = std::numeric_limits<float>::infinity();

        int stack[64];
        int stackPtr = 0;

        stack[stackPtr++] = 0;
        while (stackPtr > 0) {
            int nodeIdx = stack[--stackPtr];
            BVH::Node node = bvh.getNodes()[nodeIdx];
            float distance = AABBIntersection(origin, direction, node.min, node.max);
            if (distance < 0.0 || distance > closest + EPSILON) continue;
            if (node.left == -1 && node.right == -1) {
                for (int i = node.start; i < node.start + node.count; i++) {
                    int triangle_idx = bvh.getTriangles()[i];
                    glm::vec3 v0 = vertices[vertexIndices[3 * triangle_idx]];
                    glm::vec3 v1 = vertices[vertexIndices[3 * triangle_idx + 1]];
                    glm::vec3 v2 = vertices[vertexIndices[3 * triangle_idx + 2]];
                    distance = triangleIntersection(origin, direction, v0, v1, v2);
                    if (distance >= 0.0f) closest = std::min(closest, distance);
                }
            } else {
                if (node.left != -1) stack[stackPtr++] = node.left;
                if (node.right != -1) stack[stackPtr++] = node.right;
            }
        }
        return closest;
    }
};

TEST_F(BVHIntersectionsTest, DifferentDepthLimits) {
    glm::vec3 origin(0.5, 0.5, 0.5);
    glm::vec3 direction(0, -1, 0);
    direction = glm::normalize(direction);
    for (int depth = 1; depth < 10; depth++) {
        MedianBuilder builder(depth, -1);
        BVH bvh = builder.build(vertices, vertexIndices);
        float bvhDistance, bruteForceDistance;
        EXPECT_NO_THROW(bruteForceDistance = rayCastBruteForce(origin, direction));
        EXPECT_NO_THROW(bvhDistance = rayCastBVH(origin, direction, bvh));
        EXPECT_NEAR(bvhDistance, bruteForceDistance, EPSILON);
    }
}

TEST_F(BVHIntersectionsTest, DifferentTriangleLimits) {
    glm::vec3 origin(2, 1.8, 2);
    glm::vec3 direction(-1, -1, -1);
    direction = glm::normalize(direction);
    for (int triangles = 1; triangles < 15; triangles++) {
        MedianBuilder builder(-1, triangles);
        BVH bvh = builder.build(vertices, vertexIndices);
        float bvhDistance, bruteForceDistance;
        EXPECT_NO_THROW(bruteForceDistance = rayCastBruteForce(origin, direction));
        EXPECT_NO_THROW(bvhDistance = rayCastBVH(origin, direction, bvh));
        EXPECT_NEAR(bvhDistance, bruteForceDistance, EPSILON);
    }
}

TEST_F(BVHIntersectionsTest, DifferentRays) {
    MedianBuilder builder(-1, 3);
    BVH bvh = builder.build(vertices, vertexIndices);
    float R = 2;
    for (float phi = 0; phi < 2 * std::numbers::pi_v<float>; phi += std::numbers::pi_v<float> / 10) {
        for (float theta = 0; theta < std::numbers::pi_v<float>; theta += std::numbers::pi_v<float> / 10) {
            float x = 0.5 + R * sin(theta) * cos(phi);
            float z = 0.5 + R * sin(theta) * sin(phi);
            float y = 0.5 + R * cos(theta);

            glm::vec3 origin(x, y, z);
            glm::vec3 direction(-x, -y, -z);
            direction = glm::normalize(direction);
            float bvhDistance, bruteForceDistance;
            EXPECT_NO_THROW(bruteForceDistance = rayCastBruteForce(origin, direction));
            EXPECT_NO_THROW(bvhDistance = rayCastBVH(origin, direction, bvh));
            EXPECT_NEAR(bvhDistance, bruteForceDistance, EPSILON);
        }
    }
}

TEST_F(BVHIntersectionsTest, DifferentRaysRotated) {
    rotate(glm::radians(30.f), glm::radians(60.f), 0);
    MedianBuilder builder(-1, 1);
    BVH bvh = builder.build(vertices, vertexIndices);
    float R = 2;
    for (float phi = 0; phi < 2 * std::numbers::pi_v<float>; phi += std::numbers::pi_v<float> / 10) {
        for (float theta = 0; theta < std::numbers::pi_v<float>; theta += std::numbers::pi_v<float> / 10) {
            float x = 0.5 + R * sin(theta) * cos(phi);
            float z = 0.5 + R * sin(theta) * sin(phi);
            float y = 0.5 + R * cos(theta);

            glm::vec3 origin(x, y, z);
            glm::vec3 direction(-x, -y, -z);
            direction = glm::normalize(direction);
            float bvhDistance, bruteForceDistance;
            EXPECT_NO_THROW(bruteForceDistance = rayCastBruteForce(origin, direction));
            EXPECT_NO_THROW(bvhDistance = rayCastBVH(origin, direction, bvh));
            EXPECT_NEAR(bvhDistance, bruteForceDistance, EPSILON);
        }
    }
}