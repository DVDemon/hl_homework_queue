#include "kafka_consumer.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <exception>
#include <iostream>
#include <chrono>
#include <librdkafka/rdkafkacpp.h>

std::string Kafka_Consumer::brokers = "127.0.0.1";
std::string Kafka_Consumer::topic = "wall";

static volatile sig_atomic_t run = 1;

static bool exit_eof = false;
static int eof_cnt = 0;
static int partition_cnt = 0;
static int verbosity = 1;
static long msg_cnt = 0;
static int64_t msg_bytes = 0;

static void sigterm(int)
{
    run = 0;
}

/**
 * @brief format a string timestamp from the current time
 */
static void print_time()
{
    auto time_point = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(time_point);
    std::cerr << std::ctime(&t) << ": " << std::endl;
}

class ExampleEventCb : public RdKafka::EventCb
{
public:
    void event_cb(RdKafka::Event &event)
    {

        print_time();

        switch (event.type())
        {
        case RdKafka::Event::EVENT_ERROR:
            if (event.fatal())
            {
                std::cerr << "FATAL ";
                run = 0;
            }
            std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " << event.str() << std::endl;
            break;

        case RdKafka::Event::EVENT_STATS:
            std::cerr << "\"STATS\": " << event.str() << std::endl;
            break;

        case RdKafka::Event::EVENT_LOG:
            fprintf(stderr, "LOG-%i-%s: %s\n", event.severity(), event.fac().c_str(), event.str().c_str());
            break;

        case RdKafka::Event::EVENT_THROTTLE:
            std::cerr << "THROTTLED: " << event.throttle_time() << "ms by " << event.broker_name() << " id " << (int)event.broker_id() << std::endl;
            break;

        default:
            std::cerr << "EVENT " << event.type() << " (" << RdKafka::err2str(event.err()) << "): " << event.str() << std::endl;
            break;
        }
    }
};

void msg_consume(RdKafka::Message *message, std::function<void(const std::string&)> handler)
{
    switch (message->err())
    {
    case RdKafka::ERR__TIMED_OUT:
        break;

    case RdKafka::ERR_NO_ERROR:
        /* Real message */
        msg_cnt++;
        msg_bytes += message->len();
        if (verbosity >= 3)
            std::cerr << "Read msg at offset " << message->offset() << std::endl;
        RdKafka::MessageTimestamp ts;
        ts = message->timestamp();
        if (verbosity >= 2 &&
            ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE)
        {
            std::string tsname = "?";
            if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
                tsname = "create time";
            else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
                tsname = "log append time";
            std::cout << "Timestamp: " << tsname << " " << ts.timestamp << std::endl;
        }
        if (verbosity >= 2 && message->key())
        {
            std::cout << "Key: " << *message->key() << std::endl;
        }
        if (verbosity >= 1)
        {
            std::string val(static_cast<const char *>(message->payload()));
            handler(val);
            //std::cout << "You message [" << static_cast<const char *>(message->payload()) << "]" << std::endl;
            //printf("%.*s\n",
            //       static_cast<int>(message->len()),
            //       static_cast<const char *>(message->payload()));
        }
        break;

    case RdKafka::ERR__PARTITION_EOF:
        /* Last message */
        if (exit_eof && ++eof_cnt == partition_cnt)
        {
            std::cerr << "%% EOF reached for all " << partition_cnt << " partition(s)" << std::endl;
            run = 0;
        }
        break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
        run = 0;
        break;

    default:
        /* Errors */
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
        run = 0;
    }
}

Kafka_Consumer::Kafka_Consumer()
{
}

Kafka_Consumer Kafka_Consumer::get()
{
    static Kafka_Consumer instance;
    return instance;
}

void Kafka_Consumer::process(std::function<void(const std::string&)> handler)
{
    std::string brokers = Kafka_Consumer::brokers;
    std::string errstr;
    std::string topic_str;
    std::string mode;
    std::string debug;
    std::vector<std::string> topics = {Kafka_Consumer::topic};


    /*
   * Create configuration objects
   */
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    //ExampleRebalanceCb ex_rebalance_cb;
    //conf->set("rebalance_cb", &ex_rebalance_cb, errstr);
    conf->set("group.id",  "1", errstr); 
    conf->set("enable.partition.eof", "true", errstr);
    conf->set("metadata.broker.list", brokers, errstr);

    ExampleEventCb ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);
    conf->set("default_topic_conf", tconf, errstr);
    delete tconf;

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!consumer)
    {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }

    delete conf;

    std::cout << "% Created consumer " << consumer->name() << std::endl;


    RdKafka::ErrorCode err = consumer->subscribe(topics);
    if (err)
    {
        std::cerr << "Failed to subscribe to " << topics.size() << " topics: "
                  << RdKafka::err2str(err) << std::endl;
        exit(1);
    }


    while (run)
    {
        RdKafka::Message *msg = consumer->consume(1000);
        msg_consume(msg,handler);
        delete msg;
    }

    consumer->close();
    delete consumer;

    std::cerr << "% Consumed " << msg_cnt << " messages ("
              << msg_bytes << " bytes)" << std::endl;

    RdKafka::wait_destroyed(5000);
}