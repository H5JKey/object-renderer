#include "render-engine.hpp"
#include <print>
#include <string>
#include <fstream>
#include <sstream>
#include "target-manager.hpp"

RenderEngine::RenderEngine() {
    compileShaders();
}

void RenderEngine::compileShaders() {
    std::println("Compiling shaders...");
    std::string source;
    std::fstream shaderFile("shaders/shader.glsl");
    if (!shaderFile.is_open()) {
        throw std::runtime_error("failed to open shader file");
    }
    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    source = buffer.str();
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

    program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        throw std::runtime_error("Program linking failed");
    }

    glDeleteShader(shader);

    std::println("Compilation finished");
}

void RenderEngine::renderFrame(RenderTarget& target, const Scene& scene) const {
    if (program == 0) {
        throw std::runtime_error("Shader not compiled");
    }
    {
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

        glUseProgram(program);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vertexSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vertexIndexSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, materialSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, materialsIndexSSBO);

        glUniform1i(glGetUniformLocation(program, "uCount"), vertexIndices.size() / 3);
        glUniform3f(glGetUniformLocation(program, "uOrigin"), scene.origin.x, scene.origin.y, scene.origin.z);
        glUniform3f(glGetUniformLocation(program, "uLookAt"), scene.lookAt.x, scene.lookAt.y, scene.lookAt.z);
        glUniform3f(glGetUniformLocation(program, "uSunDirection"), scene.sunDirection.x, scene.sunDirection.y, scene.sunDirection.z);
        glUniform3f(glGetUniformLocation(program, "uBackgroundColor"), scene.backgroundColor.x, scene.backgroundColor.y, scene.backgroundColor.z);
        glUniform3f(glGetUniformLocation(program, "uSunColor"), scene.sunColor.x, scene.sunColor.y, scene.sunColor.z);
        glUniform1f(glGetUniformLocation(program, "uFovDegrees"), 90.0f);
        
        
        glBindImageTexture(0, target.getRenderTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
      
        int groupsX = (target.getWidth() + 15) / 16;
        int groupsY = (target.getHeight() + 15) / 16;
        glDispatchCompute(groupsX, groupsY, 1);

        error = glGetError();
        if (error!=0) {
            throw std::runtime_error("glDispatchCompute failed. Error: "+std::to_string(error));
        }
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        target.swapBuffers();

        glUseProgram(0);
        glDeleteBuffers(1, &vertexIndexSSBO);
        glDeleteBuffers(1, &vertexSSBO);
        glDeleteBuffers(1, &materialSSBO);
        glDeleteBuffers(1, &materialsIndexSSBO);
    }
}