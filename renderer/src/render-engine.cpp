#include "render-engine.hpp"

#include <iostream>
#include <string>

#include "render-target.hpp"
#include "utils.hpp"

RenderEngine::RenderEngine() : gen(rd()), uniformDistr(0, 0xFFFFFFFF) {
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

void RenderEngine::pathTracing(RenderTarget& target, const MeshData& meshData, const Camera& camera,
                               const glm::vec4& backgroundColor) {
    std::clog << std::format("===Path tracing started===") << std::endl;

    glUseProgram(pathTracingProgram);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vertexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vertexIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, materialSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, materialIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, bvhNodesSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, bvhTrianglesSSBO);

    glUniform1i(glGetUniformLocation(pathTracingProgram, "uCount"), meshData.vertexIndices.size() / 3);
    glUniform3f(glGetUniformLocation(pathTracingProgram, "uOrigin"), camera.origin.x, camera.origin.y, camera.origin.z);
    glUniform3f(glGetUniformLocation(pathTracingProgram, "uLookAt"), camera.lookAt.x, camera.lookAt.y, camera.lookAt.z);
    glUniform3f(glGetUniformLocation(pathTracingProgram, "uBackgroundColor"), backgroundColor.r, backgroundColor.g,
                backgroundColor.b);
    glUniform1f(glGetUniformLocation(pathTracingProgram, "uFov"), camera.fov);

    glBindImageTexture(0, target.getRawTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    int groupsX = (target.getWidth() + 15) / 16;
    int groupsY = (target.getHeight() + 15) / 16;

    const int samples = 20;
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

void RenderEngine::fillGbuffer(RenderTarget& target, const MeshData& meshData, const Camera& camera) {
    std::clog << std::format("===Filling gbuffer===") << std::endl;
    glUseProgram(gbufferProgram);
    glBindImageTexture(0, target.getNormalMap(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, target.getAlbedoMap(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vertexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, vertexIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, materialSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, materialIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, bvhNodesSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, bvhTrianglesSSBO);
    glUniform1i(glGetUniformLocation(gbufferProgram, "uCount"), meshData.vertexIndices.size() / 3);
    glUniform3f(glGetUniformLocation(gbufferProgram, "uOrigin"), camera.origin.x, camera.origin.y, camera.origin.z);
    glUniform3f(glGetUniformLocation(gbufferProgram, "uLookAt"), camera.lookAt.x, camera.lookAt.y, camera.lookAt.z);
    glUniform1f(glGetUniformLocation(gbufferProgram, "uFov"), camera.fov);

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

void RenderEngine::loadDataToGPU(const MeshData& meshData, const BVH& bvh) {
    std::clog << "Loading geometry to GPU" << std::endl;
    const auto& vertices = meshData.vertices;
    const auto& vertexIndices = meshData.vertexIndices;
    const auto& materials = meshData.materials;
    const auto& materialIndices = meshData.materialIndices;
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
    glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(Material), materials.data(), GL_STATIC_DRAW);

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

void RenderEngine::renderFrame(RenderTarget& target, const MeshData& meshData, const Camera& camera,
                               const glm::vec4& backgroundColor, const BVH& bvh) {
    if (postProcessingProgram == 0) {
        throw std::runtime_error("Shader not compiled");
    }
    RenderTarget::ContextGuard context(target);

    loadDataToGPU(meshData, bvh);
    pathTracing(target, meshData, camera, backgroundColor);

    fillGbuffer(target, meshData, camera);
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
}