#include <aws/core/Aws.h>

#include <cstdlib>
#include <exception>
#include <nlohmann/json.hpp>
#include <print>

#include "kafka-consumer.hpp"
#include "logger.hpp"
#include "render-engine.hpp"
#include "s3-client.hpp"
#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"
#include "utils.hpp"

using json = nlohmann::json;

Aws::SDKOptions options;

std::vector<uint8_t> renderPipeline(RenderEngine& engine, Scene scene, int width, int height, int samples) {
    std::shared_ptr<RenderTarget> egl = TargetManager::getInstance().createEGLTarget(width, height);
    engine.renderFrame(*egl, scene, samples);
    RenderTarget::ContextGuard guard(*egl);
    auto data = egl->getBufferData<uint8_t>(egl->getOutputTexture());
    utils::writeToPng(data, width, height, 4, "t.png");
    return utils::writeToPng(data, width, height, 4);
}

int main() try {
    Logger::getInstance().debug = true;
    Logger::getInstance().log(std::format("Aws initialized"), Logger::Level::DEBUG);
    Aws::InitAPI(options);
    TargetManager::init();
    RenderEngine engine;
    SceneLoader sceneLoader;

    Logger::getInstance().log("Renderer worker started", Logger::Level::INFO);

    KafkaConsumer consumer("127.0.0.1:9093", "renderer_worker", "create_project");
    S3Client s3client("127.0.0.1:9000", Aws::Auth::AWSCredentials("adminadmin", "adminadmin"));

    while (true) {
        std::string message = consumer.consume();
        int width, height, samples;
        std::string bucket, key;
        int rendererId;
        try {
            json json = json::parse(message);
            width = json["width"];
            height = json["height"];
            samples = json["samples"];
            bucket = json["bucket"];
            key = json["key"];
        } catch (const std::exception& e) {
            Logger::getInstance().log(std::format("Failed to parse json from string: {}", message),
                                      Logger::Level::ERROR);
            continue;
        }
        Logger::getInstance().log(
            std::format("Get data from Json: [width: {}  height: {}  samples: {}]", width, height, samples),
            Logger::Level::DEBUG);
        std::vector<uint8_t> data = s3client.getData(bucket, key);
        Scene scene = sceneLoader.loadGltfFromMemory(data);
        std::vector<uint8_t> output = renderPipeline(engine, scene, width, height, samples);
        s3client.putData(output, bucket, std::format("{}.png", key));
    }
    Logger::getInstance().log("Renderer application stopped successfully", Logger::Level::INFO);
    Aws::ShutdownAPI(options);
    TargetManager::terminate();
    return EXIT_SUCCESS;
} catch (const std::exception& e) {
    Aws::ShutdownAPI(options);
    Logger::getInstance().log("Application terminated due to error", Logger::Level::FATAL);
    TargetManager::terminate();
    return EXIT_FAILURE;
}
