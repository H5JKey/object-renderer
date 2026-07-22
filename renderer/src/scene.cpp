#include "scene.hpp"

Scene::Scene() {}

void Scene::buildMeshData() {
    meshData.vertices.clear();
    meshData.vertexIndices.clear();
    meshData.materials = this->materials;
    meshData.materialIndices.clear();

    for (const auto& mesh : meshes) {
        int indexOffset = meshData.vertices.size();
        for (const auto& v : mesh.vertices) meshData.vertices.push_back(mesh.transform * v);

        for (int i = 0; i < mesh.vertexIndices.size() / 3; i++) {
            meshData.vertexIndices.push_back(mesh.vertexIndices[i] + indexOffset);
            meshData.vertexIndices.push_back(mesh.vertexIndices[i + 1] + indexOffset);
            meshData.vertexIndices.push_back(mesh.vertexIndices[i + 2] + indexOffset);
            meshData.materialIndices.push_back(mesh.materialId);
        }
    }
}

const MeshData& Scene::getMeshData() const noexcept { return meshData; }

Camera Scene::getCamera() const noexcept { return camera; }
glm::vec4 Scene::getbackgroundColor() const noexcept { return backgroundColor; }