#pragma once
#include "render-target.hpp"
#include <vector>
#include <memory>
#include <iostream>

class TargetManager {
public:
    TargetManager(const TargetManager&) = delete;
    TargetManager(TargetManager&&) = delete;

    TargetManager& operator=(const TargetManager&) = delete;
    TargetManager& operator=(TargetManager&&) = delete;

    static TargetManager& getInstance();
    std::weak_ptr<RenderTarget> createEGLTarget(int width, int height);

    static void init();

    EGLDisplay getDisplay() const noexcept {return display;}
    EGLDisplay getContext() const noexcept {return context;}

    void initializeEGL();
private:
    TargetManager() = default;
    ~TargetManager();
private:
    EGLint majorVersion, minorVersion;
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface dummySurface;

    std::vector<std::shared_ptr<RenderTarget>> targets;
    bool initialized = false;
};