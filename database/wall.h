#ifndef WALL_H
#define WALL_H

#include "Poco/JSON/Object.h"
#include <string>
#include <vector>
#include "database_mysql.h"
#include <time.h>

namespace database
{
    struct Wall
    {
        long id;
        std::time_t time;
        std::string login;
        std::string message;

        Poco::JSON::Object::Ptr toJSON() const
        {
            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            root->set("id", id);
            root->set("time", time);
            root->set("login", login);
            root->set("message", message);
            return root;
        }

        static void init()
        {
            try
            {
                std::cout << "initializing wall: ";
                std::string query = "CREATE TABLE IF NOT EXISTS `wall` (`id` INT NOT NULL AUTO_INCREMENT,`time` DATETIME NOT NULL, `login` VARCHAR(256) NOT NULL,`message` VARCHAR(1024) NULL,  PRIMARY KEY (`id`),KEY `login` (`login`));";
                database::Database_MySQL::get().execute(query);
                std::cout << "done" << std::endl;
            }
            catch (std::exception &ex)
            {
                std::cout <<  ex.what() << std::endl;
            }
        }

        static std::vector<Wall> get(const std::string &login)
        {
            std::vector<Wall> result;
            Wall wall;

            std::string query ="SELECT id, UNIX_TIMESTAMP(time) as tt, login,message FROM wall WHERE id in ";
            query +="(select wall_id from wallview where login='";
            query +=login;
            query +="') ORDER BY id DESC;";
            database::Database_MySQL::get().query(
                query, [&](int, int column, std::string value) {
                    switch (column)
                    {
                    case 0:
                        wall.id = atoi(value.c_str());
                        break;
                    case 1:
                        wall.time = atol(value.c_str());
                        break;
                    case 2:
                        wall.login = value;
                        break;
                    case 3:
                        wall.message = value;
                        break;
                    }
                },
                [&](int) {
                    result.push_back(wall);
                });

            return result;
        }

        long insert()
        {
            std::string query = "INSERT INTO wall (time,login,message) VALUES(";
            query += "FROM_UNIXTIME(" + std::to_string(time) + "),";
            query += "'" + login + "',";
            query += "'" + message + "');";
            return database::Database_MySQL::get().execute_and_get_id(query);
        }
    };
} // namespace database
#endif