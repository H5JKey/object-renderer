#include "render-target.hpp"

#include <print>
#include <stdexcept>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

template <typename T>
std::vector<T> RenderTarget::getBufferData(GLuint texture) const {
  static_assert(sizeof(T) == 0, "Only GLubyte (RGBA8) and GLfloat (RGBA32F) are supported");
}

template <>
std::vector<float> RenderTarget::getBufferData(GLuint texture) const {
  std::vector<float> pixels(width * height * 4);
  glBindTexture(GL_TEXTURE_2D, texture);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  return pixels;
}

template <>
std::vector<unsigned char> RenderTarget::getBufferData(GLuint texture) const {
  std::vector<unsigned char> pixels(width * height * 4);
  glBindTexture(GL_TEXTURE_2D, texture);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  return pixels;
}

template <typename T>
void RenderTarget::setBufferData(GLuint texture, const std::vector<T>& data) {
  static_assert(sizeof(T) == 0, "Only GLubyte (RGBA8) and GLfloat (RGBA16F) are supported");
}

template <>
void RenderTarget::setBufferData(GLuint texture, const std::vector<float>& data) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, data.data());
  glBindTexture(GL_TEXTURE_2D, 0);
}

template <>
void RenderTarget::setBufferData(GLuint texture, const std::vector<unsigned char>& data) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
  glBindTexture(GL_TEXTURE_2D, 0);
}

EglTarget::EglTarget(int width, int height, EGLDisplay display, EGLConfig config, EGLContext context)
    : RenderTarget(width, height), display(display), context(context) {
  EGLint surfaceAttribs[] = {EGL_WIDTH, width, EGL_HEIGHT, height, EGL_NONE};
  if (!(surface = eglCreatePbufferSurface(display, config, surfaceAttribs))) {
    throw std::runtime_error("Failed to initialize EGL surface");
  }
  {
    ContextGuard context(*this);
    glGenTextures(1, &rawTexture);
    glBindTexture(GL_TEXTURE_2D, rawTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &denoisedTexture);
    glBindTexture(GL_TEXTURE_2D, denoisedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &outputTexture);
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &normalMap);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &albedoMap);
    glBindTexture(GL_TEXTURE_2D, albedoMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  initialized = true;

  std::println("EGl target created");
}

void EglTarget::output() const { writeToPng(getOutputTexture(), "output.png", GL_UNSIGNED_BYTE); }

void EglTarget::writeToPng(GLuint texture, const std::string& filename, GLenum format) const {
  std::println("Writing into {}", filename);
  ContextGuard context(*this);
  if (format == GL_FLOAT) {
    std::vector<float> pixels = getBufferData<float>(texture);
    std::vector<unsigned char> normalizedPixels(width * height * 4);
    for (int i = 0; i < pixels.size(); i++)
      normalizedPixels[i] = static_cast<unsigned char>(std::min(std::max(pixels[i], 0.0f), 1.0f) * 255);
    stbi_write_png(filename.c_str(), width, height, 4, normalizedPixels.data(), width * 4);
  } else if (format == GL_UNSIGNED_BYTE) {
    std::vector<unsigned char> pixels = getBufferData<unsigned char>(texture);
    stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), width * 4);
  } else {
    throw std::runtime_error(std::format("unknown data format:{}", format));
  }
}

void EglTarget::makeCurrent() const {
  eglMakeCurrent(display, surface, surface, context);
  EGLint error = eglGetError();
  if (error != EGL_SUCCESS) {
    throw std::runtime_error(std::format("eglMakeCurrent in EglTarget::makeCurrent failed. Error: {}", error));
  }
}

void EglTarget::release() const {
  eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  EGLint error = eglGetError();
  if (error != EGL_SUCCESS) {
    throw std::runtime_error(std::format("eglMakeCurrent in EglTarget::release failed. Error: {}", error));
  }
}

EglTarget::~EglTarget() {
  if (initialized) {
    ContextGuard guard(*this);
    glDeleteTextures(1, &denoisedTexture);
    glDeleteTextures(1, &outputTexture);
    glDeleteTextures(1, &rawTexture);
    glDeleteTextures(1, &normalMap);
    glDeleteTextures(1, &albedoMap);
  }
}