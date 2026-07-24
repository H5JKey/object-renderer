#include "scene.hpp"

#include <print>

Scene::Scene() {
    camera.origin = glm::vec4(500.0, 80.0, 0.0, 0.0);
    camera.lookAt = glm::vec4(0.0, 0.0, 0.0, 0.0);
    camera.fov = 1.57;
    backgroundColor = glm::vec4(0.0, 0.0, 0.0, 0.0);
};

void Scene::buildMeshData() {
    meshData.vertices.clear();
    meshData.vertexIndices.clear();
    meshData.materials = this->materials;
    meshData.materialIndices.clear();

    for (const auto& mesh : meshes) {
        int indexOffset = meshData.vertices.size();
        for (const auto& v : mesh.vertices) meshData.vertices.push_back(v);

        for (int i = 0; i < mesh.vertexIndices.size() / 3; i++) {
            meshData.vertexIndices.push_back(mesh.vertexIndices[3 * i] + indexOffset);
            meshData.vertexIndices.push_back(mesh.vertexIndices[3 * i + 1] + indexOffset);
            meshData.vertexIndices.push_back(mesh.vertexIndices[3 * i + 2] + indexOffset);
            meshData.materialIndices.push_back(mesh.materialId);
        }
    }
}

const Scene::MeshData& Scene::getMeshData() const noexcept { return meshData; }

Scene::Camera Scene::getCamera() const noexcept { return camera; }
glm::vec4 Scene::getbackgroundColor() const noexcept { return backgroundColor; }