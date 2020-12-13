#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

#include "web_server/kafka/kafka_consumer.h"
#include "database/wall_view.h"
#include "web_server/config.h"


namespace po = boost::program_options;

auto main(int argc, char*argv[]) -> int
{
    auto handler = [=](const std::string &value) {
        std::stringstream ss(value);
        long id;
        std::string login;
        ss >> id;
        ss >> login;
        database::WallView{0, id, login}.insert();
        std::cout << "write:" << id << " for login=" << login << std::endl;
    };

    std::string host {"127.0.0.1"};
    std::string port {"3306"};
    std::string user {"root"};
    std::string database {"hl"};
    std::string password;

    po::options_description desc{"Options"};
    desc.add_options()("help,h", "This screen")
    ("server,s",   po::value<std::string>(&host),    "set the database server")
    ("port,p",   po::value<std::string>(&port),      "set the database port")
    ("user,u",   po::value<std::string>(&user),      "set the database user")
    ("database,db",   po::value<std::string>(&database),      "set the database schema")
    ("password,p",   po::value<std::string>(&password),      "set the database password");

    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    
    Config::get().port() = port;
    Config::get().read_request_ip() = host;
    Config::get().write_request_ip() = host;
    Config::get().login() = user;
    Config::get().password() = password;
    Config::get().database() = database;
    
    Kafka_Consumer::get().process(handler);
    return 1;
}