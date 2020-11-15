#include "kafka_producer.h"
#include <librdkafka/rdkafkacpp.h>


#include "../../database/wall_view.h"
#include "../../database/friends.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <exception>

static volatile sig_atomic_t run = 1;

static void sigterm (int ) {
  run = 0;
}

class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
  void dr_cb (RdKafka::Message &message) {
    /* If message.err() is non-zero the message delivery failed permanently
     * for the message. */
    if (message.err())
      std::cerr << "% Message delivery failed: " << message.errstr() << std::endl;
    else
      std::cerr << "% Message delivered to topic " << message.topic_name() <<
        " [" << message.partition() << "] at offset " <<
        message.offset() << std::endl;
  }
};

std::string KafkaProducer::brokers  = "127.0.0.1";
std::string KafkaProducer::topic    = "wall";

KafkaProducer::KafkaProducer()
{

}

void KafkaProducer::send(long id, const std::string &login)
{
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;

    if (conf->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) 
        throw std::logic_error(errstr.c_str());
    
    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    std::vector<database::Friends> friends = database::Friends::get_friends(login);
    for (auto f : friends)
        database::WallView{0, id, f.destination_login}.insert();
    database::WallView{0, id, login}.insert();
}