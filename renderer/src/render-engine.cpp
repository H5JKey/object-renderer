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

        glUseProgram(program);
        
        error = glGetError();
        if (error!=0) {
            throw std::runtime_error("glUseProgram failed. Error: "+std::to_string(error));
        }
        glBindImageTexture(0, target.getRenderTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        error = glGetError();
        if (error != 0) {
            throw std::runtime_error("glBindImageTexture failed. Error: "+std::to_string(error));
        }
      
        int groupsX = (target.getWidth() + 7) / 8;
        int groupsY = (target.getHeight() + 7) / 8;
        glDispatchCompute(groupsX, groupsY, 1);

        error = glGetError();
        if (error!=0) {
            throw std::runtime_error("glDispatchCompute failed. Error: "+std::to_string(error));
        }
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        target.swapBuffers();

        glUseProgram(0);
    }
}

