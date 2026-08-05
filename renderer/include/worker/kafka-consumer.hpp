#pragma once
#include <cppkafka.h>

#include <optional>
#include <string>

class KafkaConsumer {
    std::string topicName;
    std::string groupId;
    std::string brokerList;
    cppkafka::Configuration config;

    cppkafka::Consumer consumer;

   public:
    std::optional<cppkafka::Message> lastMessage;
    KafkaConsumer(const std::string& brokerList, const std::string& groupId, const std::string& topicName);
    std::string consume();
    void commit();
    ~KafkaConsumer();
};