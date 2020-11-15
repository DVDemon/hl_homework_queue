#ifndef KAFKA_PRODUCER_H
#define KAFKA_PRODUCER_H

#include <string>

class KafkaProducer{
    private:
        KafkaProducer();
        static std::string brokers;
        static std::string topic;
    public:
        static KafkaProducer get(){
            static KafkaProducer instance;
            return instance;
        }

        void send(long id, const std::string& login);
};

#endif