#include <print>
#include <memory>

#include "BVH-builder.hpp"
#include "render-engine.hpp"
#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"
#include "utils.hpp"


int main(int argc, char* argv[]) {
    TargetManager::init();
    RenderEngine engine;
    std::shared_ptr<RenderTarget> egl = TargetManager::getInstance().createEGLTarget(1600, 1200);

    Scene scene;
    MedianBuilder builder(-1, 32);
    BVH bvh = builder.build(scene);
    engine.renderFrame(*egl, scene, bvh);

    auto* eglTarget = dynamic_cast<EglTarget*>(egl.get());
    if (eglTarget) {
        RenderTarget::ContextGuard guard(*egl);
        egl->output();
        utils::writeToPng(egl->getBufferData<float>(egl->getRawTexture()),egl->getWidth(), egl->getHeight(), 4, "output_raw.png");
        utils::writeToPng(egl->getBufferData<float>(egl->getAlbedoMap()), egl->getWidth(), egl->getHeight(), 4, "output_albedo.png");
        utils::writeToPng(egl->getBufferData<float>(egl->getNormalMap()), egl->getWidth(), egl->getHeight(), 4, "output_normal.png");
    }
    return EXIT_SUCCESS;
}