#include "scene-loader.hpp"

#include <stb_image.h>

#include <filesystem>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <variant>
#include <vector>

#include "logger.hpp"
#include "utils.hpp"

SceneLoader::SceneLoader() : parser(supportedExtensions) {}

Scene::TextureData SceneLoader::loadTexture(const fastgltf::Image& image, const fastgltf::Asset& asset) const {
    return std::visit(
        fastgltf::visitor{
            [&](const fastgltf::sources::URI& filePath) -> Scene::TextureData {
                Logger::getInstance().log("Loading texture from URI", Logger::Level::INFO);
                assert(filePath.fileByteOffset == 0);
                if (!filePath.uri.isLocalPath()) {
                    Logger::getInstance().log(std::format("Non-local URI not supported"), Logger::Level::ERROR);
                    throw std::runtime_error("Non-local URI not supported");
                }

                const std::string path(filePath.uri.path().begin(), filePath.uri.path().end());
                std::vector<uint8_t> result;
                int width, height, channels;
                utils::readImage(path, width, height, channels, result);

                return Scene::TextureData{.pixels = std::move(result), .width = width, .height = height};
            },
            [&](const fastgltf::sources::Array& vector) -> Scene::TextureData {
                Logger::getInstance().log(std::format("Loading texture from Array"), Logger::Level::INFO);
                std::vector<uint8_t> result;
                int width, height, channels;

                utils::readImageFromMemory(vector.bytes.data(), vector.bytes.size(), width, height, channels, result);

                return Scene::TextureData{.pixels = std::move(result), .width = width, .height = height};
            },
            [&](const fastgltf::sources::BufferView& view) -> Scene::TextureData {
                Logger::getInstance().log("Loading texture from URI", Logger::Level::INFO);
                auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                auto& buffer = asset.buffers[bufferView.bufferIndex];
                return std::visit(
                    fastgltf::visitor{
                        [&](const fastgltf::sources::Array& vector) -> Scene::TextureData {
                            std::vector<uint8_t> result;
                            int width, height, channels;
                            const auto* data = vector.bytes.data() + bufferView.byteOffset;
                            const auto size = bufferView.byteLength;

                            utils::readImageFromMemory(data, size, width, height, channels, result);
                            return Scene::TextureData{.pixels = std::move(result), .width = width, .height = height};
                        },
                        [](auto& arg) -> Scene::TextureData {
                            Logger::getInstance().log(std::string("Unsupported format: ") + typeid(arg).name(),
                                                      Logger::Level::ERROR);
                            throw std::runtime_error(std::string("Unsupported format: ") + typeid(arg).name());
                        },
                    },
                    buffer.data);
            },
            [](auto& arg) -> Scene::TextureData {
                Logger::getInstance().log(std::string("Unsupported format: ") + typeid(arg).name(),
                                          Logger::Level::ERROR);
                throw std::runtime_error(std::string("Unsupported format: ") + typeid(arg).name());
            },
        },
        image.data);
}

Scene::Material SceneLoader::loadMaterial(const fastgltf::Material& gltfMaterial,
                                          std::vector<Scene::TextureData>& textures,
                                          const fastgltf::Asset& asset) const {
    Logger::getInstance().log(std::format("Loading material {}", gltfMaterial.name), Logger::Level::INFO);
    Scene::Material material;

    material.metalness = gltfMaterial.pbrData.metallicFactor;
    material.albedo = glm::make_vec4(gltfMaterial.pbrData.baseColorFactor.data());
    material.roughness = gltfMaterial.pbrData.roughnessFactor;
    material.ior = gltfMaterial.ior;

    if (gltfMaterial.volume) {
        material.attenuationColor = glm::make_vec4(gltfMaterial.volume->attenuationColor.data());
        material.attenuationDistance = gltfMaterial.volume->attenuationDistance;
        material.thicknessFactor = gltfMaterial.volume->thicknessFactor;
    }

    material.transmission = 0.0f;
    if (gltfMaterial.transmission) {
        material.transmission = gltfMaterial.transmission->transmissionFactor;
    }

    glm::vec3 emissiveColor = glm::make_vec3(gltfMaterial.emissiveFactor.data());
    float emissiveStrength = gltfMaterial.emissiveStrength;
    glm::vec4 emission = glm::vec4(emissiveColor * emissiveStrength, 1.0f);

    material.albedoTextureID = -1;
    if (gltfMaterial.pbrData.baseColorTexture.has_value()) {
        auto& texInfo = gltfMaterial.pbrData.baseColorTexture.value();
        auto textureIndex = texInfo.textureIndex;

        if (textureIndex < asset.textures.size()) {
            auto& gltfTexture = asset.textures[textureIndex];
            auto imageIndex = gltfTexture.imageIndex;

            if (imageIndex.has_value() && imageIndex.value() < asset.images.size()) {
                auto& image = asset.images[imageIndex.value()];
                Scene::TextureData texture = loadTexture(image, asset);
                texture.id = static_cast<int>(textures.size());

                textures.push_back(std::move(texture));
                material.albedoTextureID = texture.id;
            }
        }
    }
    return material;
}

Scene::Camera SceneLoader::loadCamera(const fastgltf::Camera::Perspective& gltfCamera,
                                      const fastgltf::Node& node) const {
    Logger::getInstance().log("Loading camera", Logger::Level::INFO);
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

void SceneLoader::loadNode(const fastgltf::Node& node, const fastgltf::Asset& asset, Scene& scene,
                           glm::mat4 parentTransform) const {
    glm::mat4 localTransform = glm::mat4(1.0f);

    if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform)) {
        auto translation = glm::vec3(trs->translation.x(), trs->translation.y(), trs->translation.z());
        auto rotation = glm::quat(trs->rotation.w(), trs->rotation.x(), trs->rotation.y(), trs->rotation.z());
        auto scale = glm::vec3(trs->scale.x(), trs->scale.y(), trs->scale.z());
        localTransform = glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation) *
                         glm::scale(glm::mat4(1.0f), scale);
    }

    glm::mat4 transform = parentTransform * localTransform;

    /* Loading meshes*/
    if (node.meshIndex.has_value()) {
        const auto& gltfMesh = asset.meshes[node.meshIndex.value()];
        Scene::Mesh mesh = loadMesh(gltfMesh, asset);
        mesh.transform = transform;
        scene.meshes.push_back(std::move(mesh));
    }
    for (const auto& child : node.children) {
        loadNode(asset.nodes[child], asset, scene, transform);
    }
}

Scene::Mesh SceneLoader::loadMesh(const fastgltf::Mesh& gltfMesh, const fastgltf::Asset& asset) const {
    Scene::Mesh mesh;
    Logger::getInstance().log(std::format("Loading mesh {}", gltfMesh.name), Logger::Level::INFO);
    for (auto& gltPrimitive : gltfMesh.primitives) {
        Scene::Mesh::Primitive primitive;
        if (gltPrimitive.materialIndex.has_value())
            primitive.materialId = gltPrimitive.materialIndex.value();
        else
            primitive.materialId = 0;

        const auto* positionIt = gltPrimitive.findAttribute("POSITION");
        if (positionIt == gltPrimitive.attributes.end()) {
            Logger::getInstance().log("Failed to find POSITION attribute", Logger::Level::ERROR);
            throw std::runtime_error("Failed to find POSITION attribute");
        }
        const auto& positionAccessor = asset.accessors[positionIt->accessorIndex];
        if (!positionAccessor.bufferViewIndex.has_value()) {
            Logger::getInstance().log("No value at bufferViewIndex", Logger::Level::ERROR);
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

        const auto* texCoordIt = gltPrimitive.findAttribute("TEXCOORD_0");
        if (texCoordIt != gltPrimitive.attributes.end()) {
            const auto& texCoordAccessor = asset.accessors[texCoordIt->accessorIndex];
            if (!texCoordAccessor.bufferViewIndex.has_value()) {
                Logger::getInstance().log("No value at bufferViewIndex", Logger::Level::ERROR);
                throw std::runtime_error("No value at bufferViewIndex");
            }
            size_t startTexCoord = mesh.texCoords.size();

            mesh.texCoords.resize(startTexCoord + texCoordAccessor.count);
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                asset, texCoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx) {
                    size_t texIdx = startTexCoord + idx;
                    mesh.texCoords[texIdx] = glm::vec4(uv.x(), uv.y(), 1.0f, 1.0f);
                });
        }
        if (!gltPrimitive.indicesAccessor.has_value()) {
            Logger::getInstance().log("No value at indicesAccessor", Logger::Level::ERROR);
            throw std::runtime_error("No value at indicesAccessor");
        }
        const auto& indexAccessor = asset.accessors[gltPrimitive.indicesAccessor.value()];
        size_t startIndex = mesh.vertexIndices.size();
        primitive.startVertexIndex = startIndex;
        primitive.vertexIndicesCount = indexAccessor.count;
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
        mesh.primitives.push_back(std::move(primitive));
    }
    return mesh;
}

Scene SceneLoader::loadGltf(const std::filesystem::path& path) {
    Scene scene;
    Logger::getInstance().log(std::format("Loading scene from file {}", path.string()), Logger::Level::INFO);
    if (!std::filesystem::exists(path)) {
        Logger::getInstance().log(std::format("Failed to find {} ", path.string()), Logger::Level::ERROR);
        throw std::runtime_error(std::format("Failed to find {} ", path.string()));
    }

    auto data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.error() != fastgltf::Error::None) {
        Logger::getInstance().log(
            std::format("Failed to load {}. Error: {} ", path.string(), fastgltf::getErrorMessage(data.error())),
            Logger::Level::ERROR);
        throw std::runtime_error(
            std::format("Failed to load {}. Error: {} ", path.string(), fastgltf::getErrorMessage(data.error())));
    }

    auto asset = parser.loadGltf(data.get(), path.parent_path(), gltfOptions);
    if (auto error = asset.error(); error != fastgltf::Error::None) {
        Logger::getInstance().log(
            std::format("Failed to load {}. Error: {} ", path.string(), fastgltf::getErrorMessage(data.error())),
            Logger::Level::ERROR);
        throw std::runtime_error(
            std::format("Failed to load {}. Error: {} ", path.string(), fastgltf::getErrorMessage(data.error())));
    }

    /* Loading nodes recursively*/
    Logger::getInstance().log("Loading nodes recursively", Logger::Level::INFO);
    const fastgltf::Scene& gltfScene = asset->scenes[asset->defaultScene.value()];
    for (std::size_t rootIdx : gltfScene.nodeIndices) {
        fastgltf::Node& root = asset->nodes[rootIdx];
        loadNode(root, asset.get(), scene);
    }

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
        Logger::getInstance().log("Camera was not found in file. Default camera will be applied",
                                  Logger::Level::WARNING);
        glm::vec3 boxMax(-std::numeric_limits<float>::infinity()), boxMin(std::numeric_limits<float>::infinity());
        bool empty = true;
        for (const auto& mesh : scene.getMeshes()) {
            for (auto v : mesh.vertices) {
                v = glm::vec3(mesh.transform * glm::vec4(v, 1.0));
                empty = false;
                boxMax = glm::max(boxMax, v);
                boxMin = glm::min(boxMin, v);
            }
        }
        if (!empty) {
            glm::vec3 center = (boxMin + boxMax) / 2.0f;
            glm::vec3 size = boxMax - boxMin;
            scene.camera.fov = glm::radians(70.f);

            float maxSize = std::max(size.x, std::max(size.y, size.z));

            float distance = (maxSize * 0.5f) / std::tan(scene.camera.fov * 0.5f);

            scene.camera.origin = center + glm::normalize(glm::vec3(1.f, 0.7f, 1.0f)) * 2.0f * distance;
            scene.camera.lookAt = center;
        } else {
            scene.camera.origin = glm::vec3(1, 0, 0);
            scene.camera.lookAt = glm::vec3(0, 0, 0);
            scene.camera.fov = glm::radians(70.f);
        }
    }

    /* Loading materials */
    for (const auto& material : asset->materials) {
        scene.materials.push_back(loadMaterial(material, scene.textures, asset.get()));
    }
    return scene;
}

/* Add plane for better ligting visualization*/
void SceneLoader::addPlane(Scene& scene) {
    float lowest = std::numeric_limits<float>::infinity();
    for (const auto& mesh : scene.getMeshes()) {
        for (auto v : mesh.vertices) {
            v = glm::vec3(mesh.transform * glm::vec4(v, 1.0));
            lowest = std::min(lowest, v.y);
        }
    }

    Scene::Mesh planeMesh;
    planeMesh.transform = glm::mat4(1.0);
    planeMesh.vertices = {glm::vec3(-100.0f, lowest, 100.0f), glm::vec3(-100.0f, lowest, -100.0f),
                          glm::vec3(100.0f, lowest, -100.0f), glm::vec3(100.0f, lowest, 100.0f)};
    planeMesh.vertexIndices = {3, 1, 0, 2, 1, 3};

    Scene::Mesh::Primitive primitive1, primitive2;
    primitive1.startVertexIndex = 0;
    primitive1.vertexIndicesCount = 3;
    primitive2.startVertexIndex = 3;
    primitive2.vertexIndicesCount = 3;
    scene.materials.push_back(Scene::Material());
    primitive1.materialId = scene.materials.size() - 1;
    primitive2.materialId = scene.materials.size() - 1;

    planeMesh.primitives.push_back(primitive1);
    planeMesh.primitives.push_back(primitive2);

    scene.meshes.push_back(planeMesh);
}
