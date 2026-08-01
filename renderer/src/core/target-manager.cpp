#include "target-manager.hpp"

#include <stdexcept>

#include "logger.hpp"

TargetManager& TargetManager::getInstance() {
    static TargetManager instance;
    return instance;
}

void TargetManager::init() {
    TargetManager& self = getInstance();
    if (!self.initialized) {
        Logger::getInstance().log("Initializing EGL", Logger::Level::DEBUG);
        self.display = eglGetPlatformDisplay(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, nullptr);
        if (self.display == EGL_NO_DISPLAY) {
            Logger::getInstance().log("Failed to get EGL display", Logger::Level::FATAL);
            throw std::runtime_error("Failed to get EGL display");
        }

        if (!eglInitialize(self.display, &self.majorVersion, &self.minorVersion)) {
            Logger::getInstance().log("Failed to initialize EGL", Logger::Level::FATAL);
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
            Logger::getInstance().log("Failed to choose config", Logger::Level::FATAL);
            throw std::runtime_error("Failed to choose config");
        }

        EGLint contextAttribs[] = {
            EGL_CONTEXT_MAJOR_VERSION,           4,       EGL_CONTEXT_MINOR_VERSION, 3, EGL_CONTEXT_OPENGL_PROFILE_MASK,
            EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT, EGL_NONE};

        if (!(self.context = eglCreateContext(self.display, self.config, EGL_NO_CONTEXT, contextAttribs))) {
            EGLint error = eglGetError();
            Logger::getInstance().log("Failed to create EGL context. Error: " + std::to_string(error),
                                      Logger::Level::FATAL);
            throw std::runtime_error("Failed to create EGL context. Error: " + std::to_string(error));
        }

        EGLint surfaceAttribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
        if (!(self.dummySurface = eglCreatePbufferSurface(self.display, self.config, surfaceAttribs))) {
            Logger::getInstance().log("Failed to initialize EGL surface", Logger::Level::FATAL);
            throw std::runtime_error("Failed to initialize EGL surface");
        }

        if (!eglMakeCurrent(self.display, self.dummySurface, self.dummySurface, self.context)) {
            Logger::getInstance().log("eglMakeCurrent for dummy surface in TargetManager::init failed",
                                      Logger::Level::FATAL);
            throw std::runtime_error("eglMakeCurrent for dummy surface in TargetManager::init failed");
        }

        if (!gladLoadGL(eglGetProcAddress)) {
            Logger::getInstance().log("gladLoadGL failed", Logger::Level::FATAL);
            throw std::runtime_error("gladLoadGL failed");
        }
        Logger::getInstance().log(
            std::format("\n\tVendor   : {}\n\tVendor   : {}\n\tVersion  : {}", (const char*)glGetString(GL_VENDOR),
                        (const char*)glGetString(GL_RENDERER), (const char*)glGetString(GL_VERSION)),
            Logger::Level::DEBUG);

        self.initialized = true;
    }
}

std::shared_ptr<RenderTarget> TargetManager::createEGLTarget(int width, int height) {
    if (!initialized) {
        Logger::getInstance().log("Failed to create EGLTarget: context wasnt created", Logger::Level::ERROR);
        throw std::runtime_error("Failed to create EGLTarget: context wasnt created");
    }
    return std::shared_ptr<EglTarget>(new EglTarget(width, height, display, config, context));
}

TargetManager::~TargetManager() {
    if (initialized) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (dummySurface != EGL_NO_SURFACE) {
            eglDestroySurface(display, dummySurface);
            dummySurface = EGL_NO_SURFACE;
        }

        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
            context = EGL_NO_CONTEXT;
        }

        if (display != EGL_NO_DISPLAY) {
            eglTerminate(display);
            display = EGL_NO_DISPLAY;
        }

        initialized = false;
    }
}
