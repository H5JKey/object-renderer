#include <aws/core/Aws.h>

#include <cstdlib>
#include <exception>
#include <nlohmann/json.hpp>

#include "kafka-consumer.hpp"
#include "kafka-producer.hpp"
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
    KafkaProducer producer("127.0.0.1:9093");
    S3Client s3client("127.0.0.1:9000", Aws::Auth::AWSCredentials("adminadmin", "adminadmin"));

    while (true) {
        try {
            std::string message = consumer.consume();
            int width, height, samples;
            int project_id;
            std::string bucket, key;
            std::string modelName;
            int rendererId;
            try {
                json inputJson = json::parse(message);
                width = inputJson["width"];
                height = inputJson["height"];
                samples = inputJson["samples"];
                bucket = inputJson["bucket"];
                key = inputJson["key"];
                modelName = inputJson["name"];
                project_id = inputJson["project_id"];
            } catch (const std::exception& e) {
                Logger::getInstance().log(
                    std::format("Failed to parse json from string: {}. Error: {}", message, e.what()),
                    Logger::Level::ERROR);
                throw;
            }
            std::vector<uint8_t> data;
            Logger::getInstance().log(
                std::format("Get data from Json: [width: {}  height: {}  samples: {}]", width, height, samples),
                Logger::Level::DEBUG);

            data = s3client.getData(bucket, key);

            Scene scene = sceneLoader.loadGltfFromMemory(data);
            std::vector<uint8_t> output = renderPipeline(engine, scene, width, height, samples);

            std::string outputKey = key;
            auto dotPos = outputKey.rfind('.');
            if (dotPos != std::string::npos) {
                outputKey = outputKey.substr(0, dotPos);
            }
            outputKey += ".png";

            s3client.putData(output, "output", outputKey);

            json outputJson;
            try {
                outputJson["project_id"] = project_id;
                outputJson["bucket"] = "output";
                outputJson["key"] = outputKey;
                outputJson["name"] = modelName;
            } catch (const std::exception& e) {
                Logger::getInstance().log(std::format("Failed to generate output json: ", e.what()),
                                          Logger::Level::ERROR);
                throw;
            }

            producer.produce("generate_model", outputJson.dump());
            consumer.commit();
        } catch (const std::exception& e) {
            Logger::getInstance().log(std::format("Processing request failed: {}", e.what()), Logger::Level::ERROR);
            continue;
        }
        Logger::getInstance().log(std::format("Processing request finished successfully. Listening..."),
                                  Logger::Level::DEBUG);
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
