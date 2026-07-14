#include "render-engine.hpp"
#include <print>
#include <string>
#include <fstream>
#include <sstream>
#include "target-manager.hpp"

RenderEngine::RenderEngine() {
    std::println("Compiling path tracing shader");
    pathTracingProgram = compileShader(readFromFile("shaders/path-tracing.glsl"));
    std::println("Compiling post processing shader");
    postProcessingProgram = compileShader(readFromFile("shaders/post-processing.glsl"));
}

std::string RenderEngine::readFromFile(const std::string& path) const {
    std::string source;
    std::fstream shaderFile(path);
    if (!shaderFile.is_open()) {
        throw std::runtime_error(std::format("failed to open shader file. Path: {}", path));
    }
    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    source = buffer.str();
    return source;
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

        std::println("Shader compilation failed:");
        std::println("{}", errorLog);
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

void RenderEngine::renderFrame(RenderTarget& target, const Scene& scene) const {
    if (postProcessingProgram == 0) {
        throw std::runtime_error("Shader not compiled");
    }
    {
        /***************************/
        /****** PATH TRACING *******/
        /***************************/
        GLenum error;
        RenderTarget::ContextGuard context(target);

        GLuint vertexSSBO, vertexIndexSSBO;
        GLuint materialSSBO, materialsIndexSSBO;

        const auto& vertices = scene.vertices;
        const auto& vertexIndices = scene.vertexIndices;
        const auto& materials = scene.materials;
        const auto& materialIndices = scene.materialIndices;

        glGenBuffers(1, &vertexSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(vec3), 
                    vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &vertexIndexSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexIndexSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, vertexIndices.size() * sizeof(int), 
                    vertexIndices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &materialSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(Material), 
                    materials.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &materialsIndexSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialsIndexSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, materialIndices.size() * sizeof(int), 
                    materialIndices.data(), GL_STATIC_DRAW);

        glUseProgram(pathTracingProgram);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vertexSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vertexIndexSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, materialSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, materialsIndexSSBO);

        glUniform1i(glGetUniformLocation(pathTracingProgram, "uCount"), vertexIndices.size() / 3);
        glUniform3f(glGetUniformLocation(pathTracingProgram, "uOrigin"), scene.origin.x, scene.origin.y, scene.origin.z);
        glUniform3f(glGetUniformLocation(pathTracingProgram, "uLookAt"), scene.lookAt.x, scene.lookAt.y, scene.lookAt.z);
        glUniform3f(glGetUniformLocation(pathTracingProgram, "uSunDirection"), scene.sunDirection.x, scene.sunDirection.y, scene.sunDirection.z);
        glUniform3f(glGetUniformLocation(pathTracingProgram, "uBackgroundColor"), scene.backgroundColor.x, scene.backgroundColor.y, scene.backgroundColor.z);
        glUniform3f(glGetUniformLocation(pathTracingProgram, "uSunColor"), scene.sunColor.x, scene.sunColor.y, scene.sunColor.z);
        glUniform1f(glGetUniformLocation(pathTracingProgram, "uFovDegrees"), 90.0f);
        glUniform1ui(glGetUniformLocation(pathTracingProgram, "uSamples"), 1u);
        
        glBindImageTexture(0, target.getHDRTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

        int groupsX = (target.getWidth() + 15) / 16;
        int groupsY = (target.getHeight() + 15) / 16;
        for (int i=0; i<20; i++) {
            glUniform1ui(glGetUniformLocation(pathTracingProgram, "uSeed"), uint(rand()));
            glUniform1ui(glGetUniformLocation(pathTracingProgram, "uFrameIndex"), i);
            glDispatchCompute(groupsX, groupsY, 1);

            error = glGetError();
            if (error!=0) {
                throw std::runtime_error("glDispatchCompute for path tracing failed. Error: "+std::to_string(error));
            }
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            glFinish();
        }
        /***************************/
        /***** POST PROCESSING *****/
        /***************************/
        glUseProgram(postProcessingProgram);
        glBindImageTexture(0, target.getHDRTexture(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
        glBindImageTexture(1, target.getOutputTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        glDispatchCompute(groupsX, groupsY, 1);
        error = glGetError();
        if (error!=0) {
            throw std::runtime_error("glDispatchCompute for post processing failed. Error: "+std::to_string(error));
        }
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glFinish();

        target.swapBuffers();

        glUseProgram(0);
        glDeleteBuffers(1, &vertexIndexSSBO);
        glDeleteBuffers(1, &vertexSSBO);
        glDeleteBuffers(1, &materialSSBO);
        glDeleteBuffers(1, &materialsIndexSSBO);
    }
}