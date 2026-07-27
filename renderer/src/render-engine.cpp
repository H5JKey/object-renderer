#include "render-engine.hpp"

#include <iostream>
#include <string>

#include "render-target.hpp"
#include "utils.hpp"

RenderEngine::RenderEngine() : gen(rd()), uniformDistr(0, 0xFFFFFFFF), bvhBuilder(-1, 32) {
    std::clog << std::format("Compiling path tracing shader") << std::endl;
    pathTracingProgram = compileShader(utils::readFromFile("shaders/path-tracing.glsl"));
    std::clog << std::format("Compiling post processing shader") << std::endl;

    postProcessingProgram = compileShader(utils::readFromFile("shaders/post-processing.glsl"));
    std::clog << std::format("Compiling gbuffer shader") << std::endl;

    gbufferProgram = compileShader(utils::readFromFile("shaders/gbuffer.glsl"));
}

GLuint RenderEngine::compileShader(const std::string& source) {
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> log(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());

        std::string errorLog(log.data(), logLength);

        std::clog << std::format("Shader compilation failed:") << std::endl;
        std::clog << std::format("{}", errorLog) << std::endl;
        throw std::runtime_error("Shader compilation failed");
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        throw std::runtime_error("Program linking failed");
    }

    glDeleteShader(shader);
    return program;
}

void RenderEngine::pathTracing(RenderTarget& target, const GPUData& gpuData, const Scene::Camera& camera,
                               const glm::vec3 backgroundColor) {
    std::clog << std::format("===Path tracing started===") << std::endl;

    glUseProgram(pathTracingProgram);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vertexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, texCoordSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, vertexIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, materialSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, materialIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, bvhNodesSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, bvhTrianglesSSBO);
    glBindTextureUnit(8, textureArray);

    glUniform3f(glGetUniformLocation(pathTracingProgram, "uOrigin"), camera.origin.x, camera.origin.y, camera.origin.z);
    glUniform3f(glGetUniformLocation(pathTracingProgram, "uLookAt"), camera.lookAt.x, camera.lookAt.y, camera.lookAt.z);
    glUniform3f(glGetUniformLocation(pathTracingProgram, "uBackgroundColor"), backgroundColor.r, backgroundColor.g,
                backgroundColor.b);
    glUniform1f(glGetUniformLocation(pathTracingProgram, "uFov"), tan(camera.fov / 2.0f));

    glBindImageTexture(0, target.getRawTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    int groupsX = (target.getWidth() + 15) / 16;
    int groupsY = (target.getHeight() + 15) / 16;

    const int samples = 40;
    for (int i = 0; i < samples; i++) {
        if (i % 5 == 0) std::clog << std::format("{}/{}", i, samples) << std::endl;
        glUniform1ui(glGetUniformLocation(pathTracingProgram, "uSeed"), uniformDistr(gen));
        glUniform1ui(glGetUniformLocation(pathTracingProgram, "uFrameIndex"), i);
        glDispatchCompute(groupsX, groupsY, 1);

        GLenum error = glGetError();
        if (error != 0) {
            throw std::runtime_error("glDispatchCompute for path tracing failed. Error: " + std::to_string(error));
        }
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glFinish();
    }
    glUseProgram(0);
}

void RenderEngine::fillGbuffer(RenderTarget& target, const GPUData& gpuData, const Scene::Camera& camera) {
    std::clog << std::format("===Filling gbuffer===") << std::endl;
    glUseProgram(gbufferProgram);
    glBindImageTexture(0, target.getNormalMap(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, target.getAlbedoMap(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vertexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, texCoordSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, vertexIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, materialSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, materialIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, bvhNodesSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, bvhTrianglesSSBO);
    glBindTextureUnit(9, textureArray);

    glUniform3f(glGetUniformLocation(gbufferProgram, "uOrigin"), camera.origin.x, camera.origin.y, camera.origin.z);
    glUniform3f(glGetUniformLocation(gbufferProgram, "uLookAt"), camera.lookAt.x, camera.lookAt.y, camera.lookAt.z);
    glUniform1f(glGetUniformLocation(gbufferProgram, "uFov"), tan(camera.fov / 2.0f));

    int groupsX = (target.getWidth() + 15) / 16;
    int groupsY = (target.getHeight() + 15) / 16;
    glDispatchCompute(groupsX, groupsY, 1);
    GLenum error = glGetError();
    if (error != 0) {
        throw std::runtime_error("glDispatchCompute for gbuffer failed. Error: " + std::to_string(error));
    }
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glFinish();
    glUseProgram(0);
}

void RenderEngine::postProcess(RenderTarget& target) const {
    std::clog << "===Post processing===" << std::endl;
    glUseProgram(postProcessingProgram);
    glBindImageTexture(0, target.getDenoisedTexture(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, target.getOutputTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    int groupsX = (target.getWidth() + 15) / 16;
    int groupsY = (target.getHeight() + 15) / 16;
    glDispatchCompute(groupsX, groupsY, 1);
    GLenum error = glGetError();
    if (error != 0) {
        throw std::runtime_error("glDispatchCompute for post processing failed. Error: " + std::to_string(error));
    }
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glFinish();
    glUseProgram(0);
}

void RenderEngine::uploadGPUBuffers(const GPUData& gpuData, const BVH& bvh) {
    std::clog << "Creating and filling buffers" << std::endl;
    const auto& vertices = gpuData.vertices;
    const auto& texCoords = gpuData.texCoords;
    const auto& vertexIndices = gpuData.vertexIndices;
    const auto& materials = gpuData.materials;
    const auto& materialIndices = gpuData.materialIndices;
    const auto& bvhNodes = bvh.getNodes();
    const auto& bvhTriangles = bvh.getTriangles();
    std::clog << std::format("- Total triangles: {}", vertexIndices.size() / 3) << std::endl;
    std::clog << std::format("- BVH nodes: {}", bvhNodes.size()) << std::endl;
    std::clog << std::format("- BVH depth: {}", bvh.getDepth()) << std::endl;
    GLenum error;

    glGenBuffers(1, &vertexSSBO);
    error = glGetError();
    if (error != 0) {
        throw std::runtime_error("failed to create vertexSSBO. Error: " + std::to_string(error));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(glm::vec4), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &texCoordSSBO);
    error = glGetError();
    if (error != 0) {
        throw std::runtime_error("failed to create texCoordsSSBO. Error: " + std::to_string(error));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, texCoordSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, texCoords.size() * sizeof(glm::vec4), texCoords.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &vertexIndexSSBO);
    error = glGetError();
    if (error != 0) {
        throw std::runtime_error("failed to create vertexIndicesSSBO. Error: " + std::to_string(error));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexIndexSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, vertexIndices.size() * sizeof(int), vertexIndices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &materialSSBO);
    error = glGetError();
    if (error != 0) {
        throw std::runtime_error("failed to create materialsSSBO. Error: " + std::to_string(error));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(GPUMaterial), materials.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &materialIndexSSBO);
    error = glGetError();
    if (error != 0) {
        throw std::runtime_error("failed to create materialsIndiciesSSBO. Error: " + std::to_string(error));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialIndexSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, materialIndices.size() * sizeof(int), materialIndices.data(),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &bvhNodesSSBO);
    error = glGetError();
    if (error != 0) {
        throw std::runtime_error("failed to create bvhNodesSSBO. Error: " + std::to_string(error));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhNodesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bvhNodes.size() * sizeof(BVH::Node), bvhNodes.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &bvhTrianglesSSBO);
    error = glGetError();
    if (error != 0) {
        throw std::runtime_error("failed to create bvhTrianglesSSBO. Error: " + std::to_string(error));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhTrianglesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bvhTriangles.size() * sizeof(int), bvhTriangles.data(), GL_STATIC_DRAW);
}

RenderEngine::GPUData RenderEngine::convertSceneToGPUData(const Scene& scene) {
    std::clog << "Converting scene to GPU data" << std::endl;
    GPUData data;
    for (const auto& mesh : scene.getMeshes()) {
        int indexOffset = data.vertices.size();
        for (const auto& v : mesh.vertices) data.vertices.push_back(mesh.transform * glm::vec4(v, 1.0));
        for (const auto& uv : mesh.texCoords) data.texCoords.push_back(glm::vec4(uv.x, uv.y, 1.0f, 1.0f));

        for (const auto& primitive : mesh.primitives) {
            for (int i = primitive.startVertexIndex; i < primitive.vertexIndicesCount + primitive.startVertexIndex;
                 i++) {
                data.vertexIndices.push_back(mesh.vertexIndices[i] + indexOffset);
            }
            for (int i = 0; i < primitive.vertexIndicesCount / 3; i++) {
                data.materialIndices.push_back(primitive.materialId);
            }
        }
    }

    for (const auto& material : scene.getMaterials()) {
        GPUMaterial gpuMaterial;
        gpuMaterial.albedo = glm::vec4(material.albedo, 1.0);
        gpuMaterial.emission = glm::vec4(material.emission, 1.0);
        gpuMaterial.metalness = material.metalness;
        gpuMaterial.thicknessFactor = material.thicknessFactor;
        gpuMaterial.attenuationColor = glm::vec4(material.attenuationColor, 1.0);
        gpuMaterial.attenuationDistance = material.attenuationDistance;
        gpuMaterial.roughness = material.roughness;
        gpuMaterial.transmission = material.transmission;
        gpuMaterial.ior = material.ior;
        gpuMaterial.albedoTextureID = material.albedoTextureID;
        data.materials.push_back(gpuMaterial);
    }
    return data;
}

void RenderEngine::loadTextures(const std::vector<Scene::TextureData>& textures) {
    int maxWidth = 0;
    int maxHeight = 0;
    for (const auto& texture : textures) {
        maxWidth = std::max(maxWidth, texture.width);
        maxHeight = std::max(maxHeight, texture.height);
    }
    if (textureArray != 0) {
        glDeleteTextures(1, &textureArray);
        textureArray = 0;
    }

    glGenTextures(1, &textureArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, maxWidth, maxHeight, textures.size());

    for (const auto& texture : textures) {
        std::clog << std::format("Loading texture {} to GPU", texture.id) << std::endl;
        if (texture.pixels.empty()) {
            throw std::runtime_error("Failed to load texture. Texture has no data");
        }
        int layer = loadedTextures.size();
        loadedTextures[texture.id] = layer;

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, texture.width, texture.height, 1, GL_RGBA,
                        GL_UNSIGNED_BYTE, texture.pixels.data());
    }
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void RenderEngine::renderFrame(RenderTarget& target, const Scene& scene) {
    if (postProcessingProgram == 0) {
        throw std::runtime_error("Shader not compiled");
    }
    RenderTarget::ContextGuard context(target);
    GPUData gpuData = convertSceneToGPUData(scene);
    loadTextures(scene.getTexturesData());
    BVH bvh = bvhBuilder.build(gpuData.vertices, gpuData.vertexIndices);
    auto camera = scene.getCamera();
    auto backgroundColor = scene.getBackgroundColor();
    uploadGPUBuffers(gpuData, bvh);
    pathTracing(target, gpuData, camera, backgroundColor);

    fillGbuffer(target, gpuData, camera);
    std::clog << "===Denoising===" << std::endl;
    denoiser.denoise(target);
    postProcess(target);
}

RenderEngine::~RenderEngine() {
    glDeleteBuffers(1, &vertexSSBO);
    glDeleteBuffers(1, &vertexIndexSSBO);
    glDeleteBuffers(1, &materialSSBO);
    glDeleteBuffers(1, &materialIndexSSBO);
    glDeleteBuffers(1, &bvhNodesSSBO);
    glDeleteBuffers(1, &bvhTrianglesSSBO);
    glDeleteBuffers(1, &texCoordSSBO);
    glDeleteTextures(1, &textureArray);

    glDeleteProgram(pathTracingProgram);
    glDeleteProgram(postProcessingProgram);
    glDeleteProgram(gbufferProgram);
}