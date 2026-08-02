#include <cppkafka.h>

#include <functional>
#include <string>

class KafkaConsumer {
    std::string topicName;
    std::string groupId;
    std::string brokerList;
    cppkafka::Configuration config;

    cppkafka::Consumer consumer;

   public:
    KafkaConsumer(std::string_view brokerList, std::string_view groupId, std::string_view topicName);
    std::string consume();
    ~KafkaConsumer();
};