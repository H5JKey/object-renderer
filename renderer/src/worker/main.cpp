#include <cstdlib>
#include <exception>

#include "kafka-consumer.hpp"
#include "logger.hpp"

int main() try {
    Logger::getInstance().debug = true;

    Logger::getInstance().log("Renderer worker started", Logger::Level::INFO);

    KafkaConsumer consumer("127.0.0.1:9093", "renderer_worker", "create_project");
    while (true) {
        std::string message = consumer.consume();
    }
    Logger::getInstance().log("Renderer application stopped successfully", Logger::Level::INFO);
    return EXIT_SUCCESS;
} catch (const std::exception& e) {
    Logger::getInstance().log("Application terminated due to error", Logger::Level::FATAL);
    return EXIT_FAILURE;
}
