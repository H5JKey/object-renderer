#include "kafka-consumer.hpp"

#include "consumer.h"
#include "logger.hpp"

KafkaConsumer::KafkaConsumer(const std::string& brokerList, const std::string_view& groupId,
                             const std::string& topicName)
    : topicName(topicName),
      brokerList(brokerList),
      groupId(groupId),
      config({
          {"metadata.broker.list", this->brokerList},
          {"group.id", this->groupId},
          {"auto.offset.reset", "earliest"},
          {"broker.address.family", "v4"},
      }),
      consumer(config) {
    try {
        consumer.subscribe({std::string(topicName)});
        Logger::getInstance().log(std::format("Kafka consuming messages from topic '{}'", topicName),
                                  Logger::Level::DEBUG);
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
                Logger::getInstance().log(std::format("Kafka consumed message: {}", strMessage), Logger::Level::DEBUG);
                return strMessage;
            }
        }
    }
}

void KafkaConsumer::commit() { consumer.commit(); }