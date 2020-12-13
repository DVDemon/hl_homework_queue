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
#include <memory>

static volatile sig_atomic_t run = 1;

static void sigterm(int)
{
  run = 0;
}

class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb
{
public:
  void dr_cb(RdKafka::Message &message)
  {
    /* If message.err() is non-zero the message delivery failed permanently
     * for the message. */
    if (message.err())
      std::cerr << "% Message delivery failed: " << message.errstr() << std::endl;
    else
      std::cerr << "% Message delivered to topic " << message.topic_name() << " [" << message.partition() << "] at offset " << message.offset() << std::endl;
  }
};

std::string KafkaProducer::brokers = "127.0.0.1";
std::string KafkaProducer::topic = "wall";

KafkaProducer::KafkaProducer()
{
}

void KafkaProducer::send(long id, const std::string &login)
{
  auto conf = std::unique_ptr<RdKafka::Conf>(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
  std::string errstr;

  if (conf->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK)
    throw std::logic_error(errstr.c_str());

  signal(SIGINT, sigterm);
  signal(SIGTERM, sigterm);

  ExampleDeliveryReportCb ex_dr_cb;
  if (conf->set("dr_cb", &ex_dr_cb, errstr) != RdKafka::Conf::CONF_OK)
    throw std::logic_error(errstr.c_str());

  auto producer = std::unique_ptr<RdKafka::Producer>(RdKafka::Producer::create(conf.get(), errstr));
  if (!producer)
    throw std::logic_error(errstr.c_str());

  auto send_to_kafka = [&](long id, const std::string &login) {
    std::string line;
    line = std::to_string(id);
    line += "\n";
    line += login;
    line += "\n";
    bool retry = true;
    while (retry)
    {
      RdKafka::ErrorCode err =
          producer->produce(
              /* Topic name */
              topic,
              /* Any Partition: the builtin partitioner will be
                         * used to assign the message to a topic based
                         * on the message key, or random partition if
                         * the key is not set. */
              RdKafka::Topic::PARTITION_UA,
              /* Make a copy of the value */
              RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
              /* Value */
              const_cast<char *>(line.c_str()), line.size(),
              /* Key */
              NULL, 0,
              /* Timestamp (defaults to current time) */
              0,
              /* Message headers, if any */
              NULL,
              /* Per-message opaque value passed to
                         * delivery report */
              NULL);

      if (err != RdKafka::ERR_NO_ERROR)
      {
        std::cerr << "% Failed to produce to topic " << topic << ": " << RdKafka::err2str(err) << std::endl;
        if (err == RdKafka::ERR__QUEUE_FULL)
          producer->poll(1000 /*block for max 1000ms*/);
      }
      else
      {
        std::cerr << "% Enqueued message (" << line.size() << " bytes) "
                  << "for topic " << topic << std::endl;
        retry = false;
      }
      producer->poll(0);
    }

    std::cout << "Sended:" << line << std::endl;
  };

  std::vector<database::Friends> friends = database::Friends::get_friends(login);

  for (auto f : friends)
  {
    send_to_kafka(id, f.destination_login);
   // database::WallView{0, id, f.destination_login}.insert();
  }
  send_to_kafka(id, login);
  //database::WallView{0, id, login}.insert();

  std::cerr << "% Flushing final messages..." << std::endl;
  producer->flush(10 * 1000 /* wait for max 10 seconds */);

  if (producer->outq_len() > 0)
    std::cerr << "% " << producer->outq_len() << " message(s) were not delivered" << std::endl;
}