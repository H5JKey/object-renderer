#pragma once
#include <cppkafka.h>

#include <string>

class KafkaConsumer {
    std::string topicName;
    std::string groupId;
    std::string brokerList;
    cppkafka::Configuration config;

    cppkafka::Consumer consumer;

   public:
    KafkaConsumer(const std::string& brokerList, const std::string_view& groupId, const std::string& topicName);
    std::string consume();
    void commit();
    ~KafkaConsumer();
};