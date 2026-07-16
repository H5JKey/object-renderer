#include <stdexcept>
#include <print>
#include <vector>

#include "render-target.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

EglTarget::EglTarget(int width, int height, EGLDisplay display, EGLConfig config, EGLContext context) :
            RenderTarget(width, height), 
            display(display), 
            context(context) 
{
    EGLint surfaceAttribs[] = { 
        EGL_WIDTH, width, 
        EGL_HEIGHT, height,
        EGL_NONE 
    };
    if (!(surface = eglCreatePbufferSurface(display, config, surfaceAttribs))) {
        throw std::runtime_error("Failed to initialize EGL surface");
    }
    {
    ContextGuard context(*this);
    glGenTextures(1, &HDRTexture);
    glBindTexture(GL_TEXTURE_2D, HDRTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &outputTexture);
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &normalMap);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &albedoMap);
    glBindTexture(GL_TEXTURE_2D, albedoMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    }
    initialized = true;

    std::println("EGl target created");
}

void EglTarget::output() const {
    writeToPng(getOutputTexture(), "output.png", GL_UNSIGNED_BYTE);
}

void EglTarget::writeToPng(GLuint texture, const std::string& filename, GLenum format) const {
    std::println("Writing into {}", filename);
    ContextGuard context(*this);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (format == GL_FLOAT) {
        std::vector<float> pixels(width * height * 4);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels.data());
        std::vector<unsigned char> normalizedPixels(width * height * 4);
        for (int i=0; i<pixels.size(); i++)
            normalizedPixels[i] = static_cast<unsigned char>(std::min(std::max(pixels[i], 0.0f), 1.0f)*255);
        stbi_write_png(filename.c_str(), width, height, 4, normalizedPixels.data(), width * 4);
    }
    else if (format == GL_UNSIGNED_BYTE) {
        std::vector<unsigned char> pixels(width * height * 4);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), width * 4);
    }
    else {
        throw std::runtime_error(std::format("unknown data format:{}", format));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}


void EglTarget::makeCurrent() const {
    if (!eglMakeCurrent(display, surface, surface, context)) {
        throw std::runtime_error("eglMakeCurrent in EglTarget::makeCurrent failed");
    }
}

void EglTarget::release() const {
    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        throw std::runtime_error("eglMakeCurrent in EglTarget::releaseCurrent failed");
    }
}

EglTarget::~EglTarget() {
    if (initialized) {
        ContextGuard guard(*this);
        glDeleteTextures(1, &HDRTexture);
        glDeleteTextures(1, &outputTexture);
        glDeleteTextures(1, &normalMap);
        glDeleteTextures(1, &albedoMap);
        glDeleteFramebuffers(1, &frameBuffer);
    }
}