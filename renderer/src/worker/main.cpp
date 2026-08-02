#include <cppkafka.h>

#include <cstdlib>
#include <exception>
#include <format>

#include "logger.hpp"

int main() {
    constexpr std::string_view TopicName = "create_project";
    cppkafka::Configuration config = {
        {"metadata.broker.list", "127.0.0.1:9093"},
        {"group.id", "local_renderer_worker"},
    };
    cppkafka::Consumer consumer(config);
    try {
        try {
            consumer.subscribe({std::string(TopicName)});
        } catch (const std::exception& e) {
            Logger::getInstance().log(std::format("Failed to subscribe topic '{}'", TopicName), Logger::Level::FATAL);
            throw;
        }

        Logger::getInstance().log("Local renderer worker started", Logger::Level::INFO);
        Logger::getInstance().log(std::format("Consuming messages from topic '{}'", TopicName), Logger::Level::DEBUG);
        while (true) {
            auto message = consumer.poll();
            if (message) {
                if (message.get_error()) {
                    Logger::getInstance().log(std::format("Error: {}", message.get_error().to_string()),
                                              Logger::Level::ERROR);
                } else {
                    std::string strMessage(reinterpret_cast<const char*>(message.get_payload().get_data()),
                                           message.get_payload().get_size());
                    Logger::getInstance().log(std::format("Consume message: {}", strMessage), Logger::Level::INFO);
                }
            }
        }
        consumer.unsubscribe();
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        consumer.unsubscribe();
        Logger::getInstance().log("Application terminated due to error", Logger::Level::FATAL);
        return EXIT_FAILURE;
    }
}