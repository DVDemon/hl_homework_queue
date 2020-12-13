#ifndef KAFKA_CONSUMER_H
#define KAFKA_CONSUMER_H

#include <string>
#include <functional>

class Kafka_Consumer{
    private:
        Kafka_Consumer();
        static std::string brokers;
        static std::string topic;        
    public:
        static Kafka_Consumer get();
        void   process(std::function<void(const std::string&)> handler);

    
};

#endif