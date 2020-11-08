#ifndef DATABASEMYSQL_H
#define DATABASEMYSQL_H


#include <mysql/mysql.h>
#include <string>
#include <functional>
#include <vector>
#include <mutex>

using callback_t = std::function<void(int,int,const std::string)>;
using callback_row_t = std::function<void(int)>;

namespace database{

    enum class ConnectionType{
        READ,
        WRITE
    };

    struct Connection{
         MYSQL *_con;
         Connection(MYSQL *con) : _con(con){
         }

         Connection(const Connection& other){
             _con = other._con;
         }

         Connection& operator=(const Connection& other){
                _con = other._con;
                return *this;
         }

         ~Connection(){
             _con = nullptr;
         }
    };

    class Database_MySQL{
            private:
                Database_MySQL();
                
                std::mutex              _pool_mutex;
                std::vector<Connection> _pool;

                Connection acquire(ConnectionType ct);
                void       release(Connection& connection);

            public:
                bool query(const std::string& query,callback_t callback,callback_row_t callback_row);
                void execute(std::string& query);
                static Database_MySQL& get();
                static std::string get_version();
                void create_database(ConnectionType ct);
                ~Database_MySQL();
    };
}

#endif