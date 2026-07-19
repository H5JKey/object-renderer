#include <print>

#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"
#include "render-engine.hpp"
#include "BVH-builder.hpp"

int main(int argc, char* argv[]) {
    TargetManager::init();
    RenderEngine engine;
    std::shared_ptr<RenderTarget>  egl = TargetManager::getInstance().createEGLTarget(1600,1200);
    
    Scene scene;
    MedianBuilder builder(-1,8);
    BVH bvh = builder.build(scene);
    engine.renderFrame(*egl, scene, bvh);
    egl->output();
    auto* eglTarget = dynamic_cast<EglTarget*>(egl.get());
    if (eglTarget) {
        eglTarget->writeToPng(egl->getRawTexture(), "output_raw.png", GL_FLOAT);
        eglTarget->writeToPng(egl->getNormalMap(), "output_normal.png", GL_FLOAT);
        eglTarget->writeToPng(egl->getAlbedoMap(), "output_albedo.png", GL_FLOAT);
    }
    return EXIT_SUCCESS;
}