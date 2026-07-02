#include "render-target.hpp"
#include <stdexcept>
#include <print>

EglTarget::EglTarget(int width, int height, EGLDisplay display, EGLConfig config) : RenderTarget(width, height) {
    this->display = display;
    EGLint surfaceAttribs[] = { EGL_WIDTH, width, EGL_HEIGHT, height, EGL_NONE };
    if (!(surface = eglCreatePbufferSurface(display, config, surfaceAttribs))) {
        throw std::runtime_error("Failed to initialize EGL surface");
    }

    EGLint contextAttribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 2,
        EGL_NONE
    };

    if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs))) {
        EGLint error = eglGetError();
        std::println("{}",error);
        throw std::runtime_error("Failed to create EGL context");
    }
    makeCurrent();

    if (!gladLoadGL(eglGetProcAddress)) {
        throw std::runtime_error("gladLoadGL failed");
    }

    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("FBO creation failed");
    }

    glViewport(0, 0, width, height);

    std::println("EGl target created");
    releaseCurrent();
}

void EglTarget::swapBuffers() {
    
}

void EglTarget::makeCurrent() {
    if (!eglMakeCurrent(display, surface, surface, context)) {
        throw std::runtime_error("eglMakeCurrent in EglTarget::makeCurrent failed");
    }
}

void EglTarget::releaseCurrent() {
    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        throw std::runtime_error("eglMakeCurrent in EglTarget::releaseCurrent failed");
    }
}


GLuint EglTarget::getRenderTexture() const {
    
}

EglTarget::~EglTarget() {
    
}
