#include <gtest/gtest.h>

#include "scene-loader.hpp"
#include "scene.hpp"

TEST(SceneLoaderTest, throwsOnNonexistentFile) {
    SceneLoader loader;

    /* Failed to find non/existent/scene.gltf */
    EXPECT_THROW(loader.loadGltf("non/existent/scene.gltf"), std::runtime_error);
}

TEST(SceneLoaderTest, throwsOnInvalidFile) {
    SceneLoader loader;

    /* Failed to load data/test-image.png */
    EXPECT_THROW(loader.loadGltf("tests/data/test-image.jpg"), std::runtime_error);
}
#include <print>
TEST(SceneLoaderTest, loadSceneCorrectly) {
    SceneLoader loader;

    Scene scene;
    EXPECT_NO_THROW(scene = loader.loadGltf("tests/data/test-scene.glb"));

    const auto& meshes = scene.getMeshes();
    ASSERT_EQ(meshes.size(), 1);
    const auto& mesh = meshes[0];

    std::vector<glm::vec3> expectedVertices = {glm::vec3(0.0, 0.0, 0.0)};
    std::println("Vertex");
    for (int i = 0; i < mesh.vertices.size(); i++) {
        std::println("{} {} {}", mesh.vertices[i].x, mesh.vertices[i].y, mesh.vertices[i].z);
    }
    std::println("Index");
    for (int i = 0; i < mesh.vertexIndices.size(); i++) {
        std::println("{}", mesh.vertexIndices[i]);
    }
    std::println("TexCoord");
    for (int i = 0; i < mesh.texCoords.size(); i++) {
        std::println("{} {}", mesh.texCoords[i].x, mesh.texCoords[i].y);
    }
}