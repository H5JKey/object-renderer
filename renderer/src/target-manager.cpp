#include "target-manager.hpp"
#include <stdexcept>
#include <print>

TargetManager& TargetManager::getInstance() {
    static TargetManager instance;
    return instance;
}

std::weak_ptr<RenderTarget> TargetManager::createEGLTarget(int width, int height) {
    if (!initialized) {
        std::println("Initializing EGL...");
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            throw std::runtime_error("Failed to get display");
        }

        if (!eglInitialize(display, &majorVersion, &minorVersion)) {
             throw std::runtime_error("Failed to initialize EGL");
        }

        EGLint configAttribs[] = { EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_NONE };
        EGLint numConfigs;
        if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
            throw std::runtime_error("Failed to choose config");
        }
        std::println("EGL initialization finished");
        initialized = true;
    }
    auto target = std::shared_ptr<EglTarget>(new EglTarget(width, height, display, config));
    targets.push_back(target);
    return target;
}

TargetManager::~TargetManager() {
    if (initialized) {
        targets.clear();
        eglTerminate(display);
        display = EGL_NO_DISPLAY;
        initialized = false;
        std::println("EGL released");
    }
}