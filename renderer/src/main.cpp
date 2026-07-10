#include <print>

#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"
#include "render-engine.hpp"

int main(int argc, char* argv[]) {
    TargetManager::init();
    RenderEngine engine;
    std::shared_ptr<RenderTarget>  egl = TargetManager::getInstance().createEGLTarget(1600,1200).lock();
    
    Scene scene;
    engine.renderFrame(*egl, scene);
}