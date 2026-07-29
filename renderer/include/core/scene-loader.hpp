#pragma once
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <filesystem>
#include <string>

#include "scene.hpp"
class SceneLoader {
    static constexpr auto supportedExtensions =
        fastgltf::Extensions::KHR_mesh_quantization | fastgltf::Extensions::KHR_texture_transform |
        fastgltf::Extensions::KHR_materials_variants | fastgltf::Extensions::KHR_materials_transmission;
    static constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember |
                                        fastgltf::Options::AllowDouble | fastgltf::Options::LoadExternalBuffers |
                                        fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices |
                                        fastgltf::Options::DecomposeNodeMatrices;
    fastgltf::Parser parser;

   public:
    SceneLoader();

    Scene::TextureData loadTexture(const fastgltf::Image& image, const fastgltf::Asset& asset) const;
    Scene::Camera loadCamera(const fastgltf::Camera::Perspective& gltfCamera, const fastgltf::Node& node) const;
    Scene::Material loadMaterial(const fastgltf::Material& gltfMaterial, std::vector<Scene::TextureData>& textures,
                                 const fastgltf::Asset& asset) const;
    void loadNode(const fastgltf::Node& node, const fastgltf::Asset& asset, Scene& scene,
                  glm::mat4 parentTransform = glm::mat4(1.0f)) const;
    Scene::Mesh loadMesh(const fastgltf::Mesh& gltfMesh, const fastgltf::Asset& asset) const;
    Scene loadGltf(const std::filesystem::path& path);

    void addPlane(Scene& scene);
    ~SceneLoader() = default;
};