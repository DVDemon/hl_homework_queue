#ifndef WALLVIEW_H
#define WALLVIEW_H

#include "Poco/JSON/Object.h"
#include <string>
#include <vector>
#include <time.h>
#include "wall.h"
#include "database_mysql.h"


namespace database
{
    struct WallView
    {
        long id;
        long wall_id;
        std::string login;


        Poco::JSON::Object::Ptr toJSON() const
        {
            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            root->set("id", id);
            root->set("wall_id", wall_id);
            root->set("login", login);
            return root;
        }

        static void init()
        {
            try
            {
                std::cout << "initializing wall: ";
                std::string query = "CREATE TABLE wallview (id int NOT NULL AUTO_INCREMENT, wall_id int NOT NULL, login varchar(256) NOT NULL, PRIMARY KEY (id), UNIQUE KEY id_UNIQUE (id), KEY login_ind (login));";
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

            std::string query = "SELECT id, UNIX_TIMESTAMP(time) as tt, login,message FROM wall WHERE id in ";
            query += "(select wall_id from wallview where login='";
            query += login;
            query += "') ORDER BY id DESC;";
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

        void insert()
        {
            std::string query = "INSERT INTO wallview (wall_id,login) VALUES(";
            query += std::to_string(wall_id);
            query += ",";
            query += "'" + login + "')";
            database::Database_MySQL::get().execute(query);
        }
    };
} // namespace database
#endif