#include "scene.hpp"

#include <print>

Scene::Scene() { backgroundColor = glm::vec3(0.3f, 0.3f, 0.45f); };

Scene::Camera Scene::getCamera() const noexcept { return camera; }
glm::vec3 Scene::getBackgroundColor() const noexcept { return backgroundColor; }
const std::vector<Scene::Mesh>& Scene::getMeshes() const noexcept { return meshes; }
const std::vector<Scene::Material>& Scene::getMaterials() const noexcept { return materials; }
const std::vector<Scene::TextureData>& Scene::getTexturesData() const noexcept { return textures; }