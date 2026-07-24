#include "scene-loader.hpp"

#include <filesystem>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

SceneLoader::SceneLoader() : parser(supportedExtensions) {}

Scene::Material SceneLoader::loadMaterial(const fastgltf::Material& gltfMaterial) const {
    Scene::Material material;

    material.metalness = gltfMaterial.pbrData.metallicFactor;
    material.albedo = glm::make_vec4(gltfMaterial.pbrData.baseColorFactor.data());
    material.roughness = gltfMaterial.pbrData.roughnessFactor;
    material.ior = gltfMaterial.ior;

    material.transmission = 0.0f;
    if (gltfMaterial.transmission) {
        material.transmission = gltfMaterial.transmission->transmissionFactor;
    }

    glm::vec3 emissiveColor = glm::make_vec3(gltfMaterial.emissiveFactor.data());
    float emissiveStrength = gltfMaterial.emissiveStrength;
    glm::vec4 emission = glm::vec4(emissiveColor * emissiveStrength, 1.0f);
    return material;
}

Scene::Camera SceneLoader::loadCamera(const fastgltf::Camera::Perspective& gltfCamera,
                                      const fastgltf::Node& node) const {
    Scene::Camera camera;

    glm::mat4 worldMatrix(1.0f);
    if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform)) {
        auto translation = glm::vec3(trs->translation.x(), trs->translation.y(), trs->translation.z());
        auto rotation = glm::quat(trs->rotation.w(), trs->rotation.x(), trs->rotation.y(), trs->rotation.z());
        auto scale = glm::vec3(trs->scale.x(), trs->scale.y(), trs->scale.z());
        worldMatrix = glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation) *
                      glm::scale(glm::mat4(1.0f), scale);
    }
    camera.origin = glm::vec4(worldMatrix[3].x, worldMatrix[3].y, worldMatrix[3].z, 1.0);

    camera.fov = gltfCamera.yfov;
    glm::vec3 direction = glm::mat3(worldMatrix) * glm::vec3(0.0f, 0.0f, -1.0f);
    direction = glm::normalize(direction);

    camera.lookAt = glm::vec4(glm::vec3(camera.origin) + direction, 1.0);

    return camera;
}

Scene::Mesh SceneLoader::loadMesh(const fastgltf::Mesh& gltfMesh, const fastgltf::Asset& asset) const {
    Scene::Mesh mesh;
    for (auto& primitive : gltfMesh.primitives) {
        const auto* positionIt = primitive.findAttribute("POSITION");
        if (positionIt == primitive.attributes.end()) {
            throw std::runtime_error("Failed to find POSITION attribute");
        }
        const auto& positionAccessor = asset.accessors[positionIt->accessorIndex];
        if (!positionAccessor.bufferViewIndex.has_value()) {
            throw std::runtime_error("No value at bufferViewIndex");
        }
        size_t vertexCount = positionAccessor.count;
        size_t startVertex = mesh.vertices.size();
        mesh.vertices.resize(startVertex + vertexCount);
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
            asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
                size_t vertexIdx = startVertex + idx;
                mesh.vertices[vertexIdx] = glm::vec4(pos.x(), pos.y(), pos.z(), 1.0f);
            });

        if (!primitive.indicesAccessor.has_value()) {
            throw std::runtime_error("No value at indicesAccessor");
        }
        const auto& indexAccessor = asset.accessors[primitive.indicesAccessor.value()];
        size_t startIndex = mesh.vertexIndices.size();
        mesh.vertexIndices.resize(startIndex + indexAccessor.count);

        if (indexAccessor.componentType == fastgltf::ComponentType::UnsignedShort) {
            std::vector<uint16_t> tmp(indexAccessor.count);
            fastgltf::copyFromAccessor<uint16_t>(asset, indexAccessor, tmp.data());
            for (size_t i = 0; i < indexAccessor.count; ++i) {
                mesh.vertexIndices[startIndex + i] = tmp[i] + startVertex;
            }
        } else {
            std::vector<uint32_t> tmp(indexAccessor.count);
            fastgltf::copyFromAccessor<uint32_t>(asset, indexAccessor, tmp.data());
            for (size_t i = 0; i < indexAccessor.count; ++i) {
                mesh.vertexIndices[startIndex + i] = tmp[i] + startVertex;
            }
        }
        if (primitive.materialIndex.has_value()) {
            mesh.materialId = primitive.materialIndex.value();
        }
    }
    return mesh;
}

Scene SceneLoader::loadGltf(const std::filesystem::path& path) {
    Scene scene;

    if (!std::filesystem::exists(path)) throw std::runtime_error(std::format("Failed to find {} ", path.string()));

    std::clog << "Loading " << path << std::endl;

    auto data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.error() != fastgltf::Error::None)
        throw std::runtime_error(
            std::format("Failed to load {}. Error: {} ", path.string(), fastgltf::getErrorMessage(data.error())));

    auto asset = parser.loadGltf(data.get(), path.parent_path(), gltfOptions);
    if (auto error = asset.error(); error != fastgltf::Error::None)
        throw std::runtime_error(
            std::format("Failed to load {}. Error: {} ", path.string(), fastgltf::getErrorMessage(asset.error())));

    /* Loading meshes*/
    for (const auto& mesh : asset->meshes) scene.meshes.push_back(std::move(loadMesh(mesh, asset.get())));

    /* Loading camera */
    bool cameraFound = false;
    for (auto node : asset->nodes) {
        if (node.cameraIndex.has_value()) {
            auto* gltfCamera =
                std::get_if<fastgltf::Camera::Perspective>(&asset->cameras[node.cameraIndex.value()].camera);
            if (gltfCamera) {
                scene.camera = loadCamera(*gltfCamera, node);
                cameraFound = true;
                break;
            }
        }
    }
    if (!cameraFound) {
        std::clog << "Camera was not found in file. Default camera will be applied" << std::endl;
    }

    /* Loading materials */
    for (const auto& material : asset.get().materials) {
        scene.materials.push_back(loadMaterial(material));
    }
    return scene;
}