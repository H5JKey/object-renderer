#include "kafka-consumer.hpp"

#include "consumer.h"
#include "logger.hpp"

KafkaConsumer::KafkaConsumer(std::string_view brokerList, std::string_view groupId, std::string_view topicName)
    : topicName(topicName),
      brokerList(brokerList),
      groupId(groupId),
      config({
          {"metadata.broker.list", this->brokerList},
          {"group.id", this->groupId},
          {"auto.offset.reset", "earliest"},
          {"enable.auto.commit", "true"},
          {"auto.commit.interval.ms", "1000"},
          {"broker.address.family", "v4"},
      }),
      consumer(config) {
    try {
        consumer.subscribe({std::string(topicName)});
        Logger::getInstance().log(std::format("Consuming messages from topic '{}'", topicName), Logger::Level::DEBUG);
    } catch (const std::exception& e) {
        Logger::getInstance().log(std::format("Failed to subscribe topic '{}'", topicName), Logger::Level::FATAL);
        throw;
    }
}

KafkaConsumer::~KafkaConsumer() { consumer.unsubscribe(); }

std::string KafkaConsumer::consume() {
    while (true) {
        auto message = consumer.poll(std::chrono::milliseconds(1000));
        if (message) {
            if (message.get_error()) {
                Logger::getInstance().log(std::format("Error: {}", message.get_error().to_string()),
                                          Logger::Level::ERROR);
            } else {
                std::string strMessage(reinterpret_cast<const char*>(message.get_payload().get_data()),
                                       message.get_payload().get_size());
                Logger::getInstance().log(std::format("Consume message: {}", strMessage), Logger::Level::INFO);
                return strMessage;
            }
        }
    }
}