#include "scene.hpp"

#include <print>

Scene::Scene() {
    camera.origin = glm::vec3(-2.0, -2.0, 1);
    camera.lookAt = glm::vec3(-0.0, 0.0, 0.6);
    camera.fov = 1.57;
    backgroundColor = glm::vec3(0.7f, 0.7f, 1.0f);
};

Scene::Camera Scene::getCamera() const noexcept { return camera; }
glm::vec3 Scene::getBackgroundColor() const noexcept { return backgroundColor; }
const std::vector<Scene::Mesh>& Scene::getMeshes() const noexcept { return meshes; }
const std::vector<Scene::Material>& Scene::getMaterials() const noexcept { return materials; }
const std::vector<Scene::TextureData>& Scene::getTexturesData() const noexcept { return textures; }