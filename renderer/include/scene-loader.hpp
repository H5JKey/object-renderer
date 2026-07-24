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

    Scene::Camera loadCamera(const fastgltf::Camera::Perspective& gltfCamera, const fastgltf::Node& node) const;
    Scene::Material loadMaterial(const fastgltf::Material& gltfMaterial) const;
    Scene::Mesh loadMesh(const fastgltf::Mesh& gltfMesh, const fastgltf::Asset& asset) const;
    Scene loadGltf(const std::filesystem::path& path);

    ~SceneLoader() = default;
};