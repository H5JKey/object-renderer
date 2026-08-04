#include "kafka-producer.hpp"

#include "logger.hpp"
#include "message_builder.h"

KafkaProducer::KafkaProducer(const std::string& brokerList)
    : brokerList(brokerList),
      config({
          {"metadata.broker.list", this->brokerList},
          {"broker.address.family", "v4"},
      }),
      producer(config) {}

KafkaProducer::~KafkaProducer() {}

void KafkaProducer::produce(const std::string& topicName, const std::string& strMessage) {
    try {
        cppkafka::MessageBuilder builder(topicName);
        builder.payload(strMessage);
        producer.produce(builder);
        producer.flush();
        Logger::getInstance().log(std::format("Kafka produced message: {}", strMessage), Logger::Level::DEBUG);
    } catch (const cppkafka::HandleException& e) {
        Logger::getInstance().log(std::format("Failed to produce message: {}", e.what()), Logger::Level::ERROR);
        throw;
    }
}