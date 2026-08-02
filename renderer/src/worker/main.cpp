#include <cstdlib>
#include <exception>

#include "kafka-consumer.hpp"
#include "logger.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;



void renderPipeline(int width, int height, int samples) {
    /* Some render */
}


int main() try {
    Logger::getInstance().debug = true;

    Logger::getInstance().log("Renderer worker started", Logger::Level::INFO);

    KafkaConsumer consumer("127.0.0.1:9093", "renderer_worker", "create_project");
    while (true) {
        std::string message = consumer.consume();
        int width, height, samples;
        try {
            json json = json::parse(message);
            width = json["width"];
            height = json["height"];
            samples = json["samples"];
        } catch (const std::exception& e) {
            Logger::getInstance().log(std::format("Failed to create or parse json from string: {}", message), Logger::Level::ERROR);
        }
        Logger::getInstance().log(std::format("Get data from Json: [width: {}  height: {}  samples: {}]", width, height, samples) , Logger::Level::DEBUG);
        renderPipeline(width, height, samples);
    }
    Logger::getInstance().log("Renderer application stopped successfully", Logger::Level::INFO);
    return EXIT_SUCCESS;
} catch (const std::exception& e) {
    Logger::getInstance().log("Application terminated due to error", Logger::Level::FATAL);
    return EXIT_FAILURE;
}
