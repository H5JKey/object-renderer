#include <stdexcept>
#include <print>
#include <vector>

#include "render-target.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

EglTarget::EglTarget(int width, int height, EGLDisplay display, EGLConfig config, EGLContext context) : RenderTarget(width, height) {
    this->display = display;
    this->context = context;
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

void EglTarget::swapBuffers() const {
    std::println("writing into {}", "output.png");
    glBindTexture(GL_TEXTURE_2D, getOutputTexture());
    std::vector<unsigned char> pixels(width * height * 4);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    stbi_write_png("output.png", width, height, 4, pixels.data(), width * 4);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void EglTarget::makeCurrent() {
    if (!eglMakeCurrent(display, surface, surface, context)) {
        throw std::runtime_error("eglMakeCurrent in EglTarget::makeCurrent failed");
    }
}

void EglTarget::release() {
    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        throw std::runtime_error("eglMakeCurrent in EglTarget::releaseCurrent failed");
    }
}

EglTarget::~EglTarget() {
    
}