#include "logger.hpp"
#include "render-engine.hpp"
#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"
#include "utils.hpp"

int main(int argc, char* argv[]) try {
    if (argc != 2) return EXIT_FAILURE;
    Logger::getInstance().log("Renderer application started", Logger::Level::INFO);
    TargetManager::init();
    RenderEngine engine;
    SceneLoader loader;
    Scene scene = loader.loadGltf(argv[1]);
    loader.addPlane(scene);
    std::shared_ptr<RenderTarget> egl = TargetManager::getInstance().createEGLTarget(300, 300);
    engine.renderFrame(*egl, scene, 30);

    auto* eglTarget = dynamic_cast<EglTarget*>(egl.get());
    if (eglTarget) {
        RenderTarget::ContextGuard guard(*egl);
        Logger::getInstance().log("Writing into output.png", Logger::Level::DEBUG);
        utils::writeToPng(egl->getBufferData<uint8_t>(egl->getOutputTexture()), egl->getWidth(), egl->getHeight(), 4,
                          "output.png");
        Logger::getInstance().log("Writing into output_raw.png", Logger::Level::DEBUG);
        utils::writeToPng(egl->getBufferData<float>(egl->getRawTexture()), egl->getWidth(), egl->getHeight(), 4,
                          "output_raw.png");
        Logger::getInstance().log("Writing into output_albedo.png", Logger::Level::DEBUG);
        utils::writeToPng(egl->getBufferData<float>(egl->getAlbedoMap()), egl->getWidth(), egl->getHeight(), 4,
                          "output_albedo.png");
        Logger::getInstance().log("Writing into output_normal.png", Logger::Level::DEBUG);
        utils::writeToPng(egl->getBufferData<float>(egl->getNormalMap()), egl->getWidth(), egl->getHeight(), 4,
                          "output_normal.png");
    }
    Logger::getInstance().log("Renderer application stopped successfully", Logger::Level::INFO);
    return EXIT_SUCCESS;
} catch (const std::exception& e) {
    Logger::getInstance().log("Application terminated due to error", Logger::Level::FATAL);
    return EXIT_FAILURE;
}