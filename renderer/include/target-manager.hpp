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

    void initializeEGL();
private:
    TargetManager() = default;
    ~TargetManager();
private:
    EGLint majorVersion, minorVersion;
    EGLDisplay display;
    EGLConfig config;

    std::vector<std::shared_ptr<RenderTarget>> targets;
    bool initialized = false;
};