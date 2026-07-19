#include "target-manager.hpp"

#include <print>
#include <stdexcept>

TargetManager& TargetManager::getInstance() {
  static TargetManager instance;
  return instance;
}

void TargetManager::init() {
  TargetManager& self = getInstance();
  if (!self.initialized) {
    std::println("Initializing EGL");
    self.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (self.display == EGL_NO_DISPLAY) {
      throw std::runtime_error("Failed to get display");
    }

    if (!eglInitialize(self.display, &self.majorVersion, &self.minorVersion)) {
      throw std::runtime_error("Failed to initialize EGL");
    }

    eglBindAPI(EGL_OPENGL_API);

    EGLint configAttribs[] = {EGL_SURFACE_TYPE,
                              EGL_PBUFFER_BIT,
                              EGL_RENDERABLE_TYPE,
                              EGL_OPENGL_BIT,
                              EGL_RED_SIZE,
                              8,
                              EGL_GREEN_SIZE,
                              8,
                              EGL_BLUE_SIZE,
                              8,
                              EGL_ALPHA_SIZE,
                              8,
                              EGL_NONE};

    EGLint numConfigs;
    if (!eglChooseConfig(self.display, configAttribs, &self.config, 1, &numConfigs)) {
      throw std::runtime_error("Failed to choose config");
    }

    EGLint contextAttribs[] = {
        EGL_CONTEXT_MAJOR_VERSION,           4,       EGL_CONTEXT_MINOR_VERSION, 3, EGL_CONTEXT_OPENGL_PROFILE_MASK,
        EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT, EGL_NONE};

    if (!(self.context = eglCreateContext(self.display, self.config, EGL_NO_CONTEXT, contextAttribs))) {
      EGLint error = eglGetError();
      throw std::runtime_error("Failed to create EGL context. Error: " + std::to_string(error));
    }

    EGLint surfaceAttribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
    if (!(self.dummySurface = eglCreatePbufferSurface(self.display, self.config, surfaceAttribs))) {
      throw std::runtime_error("Failed to initialize EGL surface");
    }

    if (!eglMakeCurrent(self.display, self.dummySurface, self.dummySurface, self.context)) {
      throw std::runtime_error("eglMakeCurrent for dummy surface in TargetManager::init failed");
    }

    if (!gladLoadGL(eglGetProcAddress)) {
      throw std::runtime_error("gladLoadGL failed");
    }
    std::println("Vendor   : {}", (const char*)glGetString(GL_VENDOR));
    std::println("Renderer : {}", (const char*)glGetString(GL_RENDERER));
    std::println("Version  : {}", (const char*)glGetString(GL_VERSION));

    self.initialized = true;
  }
}

std::shared_ptr<RenderTarget> TargetManager::createEGLTarget(int width, int height) {
  if (!initialized) {
    throw std::runtime_error("Failed to create EGLTarget: context wasnt created");
  }
  return std::shared_ptr<EglTarget>(new EglTarget(width, height, display, config, context));
  ;
}

TargetManager::~TargetManager() {
  if (initialized) {
    eglTerminate(display);
    display = EGL_NO_DISPLAY;
    initialized = false;
    std::println("EGL released");
  }
}