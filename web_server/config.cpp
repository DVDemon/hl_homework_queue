#include "config.h"

Config::Config()
{
    _read_request_ip = "127.0.0.1";
    _write_request_ip = "127.0.0.1";
    _port = "3306";
    _database = "hl";
    _login = "test";
    _password = "pzjqUkMnc7vfNHET";
}

Config &Config::get()
{
    static Config _instance;
    return _instance;
}

const std::string &Config::get_port()
{
    return _port;
}

const std::string &Config::get_read_request_ip()
{
    return _read_request_ip;
}

const std::string &Config::get_write_request_ip()
{
    return _write_request_ip;
}

const std::string &Config::get_login()
{
    return _login;
}
const std::string &Config::get_password()
{
    return _password;
}
const std::string &Config::get_database()
{
    return _database;
}

std::string &Config::port()
{
    return _port;
}

std::string &Config::read_request_ip()
{
    return _read_request_ip;
}

std::string &Config::write_request_ip()
{
    return _write_request_ip;
}

std::string &Config::login()
{
    return _login;
}

std::string &Config::password()
{
    return _password;
}

std::string &Config::database()
{
    return _database;
}