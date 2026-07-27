#include "render-engine.hpp"
#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"
#include "utils.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) return EXIT_FAILURE;
    TargetManager::init();
    RenderEngine engine;
    SceneLoader loader;
    Scene scene = loader.loadGltf(argv[1]);
    std::shared_ptr<RenderTarget> egl = TargetManager::getInstance().createEGLTarget(1600, 1200);
    engine.renderFrame(*egl, scene, 10);

    auto* eglTarget = dynamic_cast<EglTarget*>(egl.get());
    if (eglTarget) {
        RenderTarget::ContextGuard guard(*egl);
        utils::writeToPng(egl->getBufferData<uint8_t>(egl->getOutputTexture()), egl->getWidth(), egl->getHeight(), 4,
                          "output.png");
        utils::writeToPng(egl->getBufferData<float>(egl->getRawTexture()), egl->getWidth(), egl->getHeight(), 4,
                          "output_raw.png");
        utils::writeToPng(egl->getBufferData<float>(egl->getAlbedoMap()), egl->getWidth(), egl->getHeight(), 4,
                          "output_albedo.png");
        utils::writeToPng(egl->getBufferData<float>(egl->getNormalMap()), egl->getWidth(), egl->getHeight(), 4,
                          "output_normal.png");
    }
}