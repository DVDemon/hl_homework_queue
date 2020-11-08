#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class  Config{
    private:
        Config();
        std::string _read_request_ip;
        std::string _write_request_ip;
        std::string _port;
        std::string _login;
        std::string _password;
        std::string _database;
    public:
        static Config& get();

        std::string& port();
        std::string& read_request_ip();
        std::string& write_request_ip();
        std::string& login();
        std::string& password();
        std::string& database();

        const std::string& get_port();
        const std::string& get_read_request_ip();
        const std::string& get_write_request_ip();
        const std::string& get_login();
        const std::string& get_password();
        const std::string& get_database();
};

#endif