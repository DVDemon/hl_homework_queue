#ifndef FRIENDS_H
#define FRIENDS_H

#include "Poco/JSON/Object.h"
#include <string>
#include <vector>
#include "database_mysql.h"

namespace database{
    struct Friends{
        std::string source_login;
        std::string destination_login;

        static void init(){
                try{
                    std::cout << "initializing friends ... " << std::endl;
                    std::string query = "CREATE TABLE friends (`id` int NOT NULL AUTO_INCREMENT,`source_login` VARCHAR(256) NOT NULL,`destination_login` VARCHAR(256) NOT NULL,PRIMARY KEY (`id`))";
                    database::Database_MySQL::get().execute(query);
                    std::cout << "initializing friends ... done" << std::endl;
                }catch(...){
                    std::cout << "initializing friends ... fail" << std::endl;
                }

            }



        Poco::JSON::Object::Ptr toJSON() const{
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

                root->set("source_login",source_login);
                root->set("destination_login",destination_login);

                return root;
            }

        static std::vector<Friends> get_friends(const std::string& source_login){
                std::vector<Friends> result;
                Friends record;

                record.source_login = source_login;
                std::string query="SELECT destination_login FROM friends WHERE source_login='"+source_login+"'";
                database::Database_MySQL::get().query(query,[&](int ,int column,std::string value)
                {
                        switch(column){
                            case 0: record.destination_login = value; break;
                            
                        }
                    
                },[&](int){
                    result.push_back(record);
                });
                
                return result;
            }
         
         

         void insert(){
            std::string query;

            query = "INSERT INTO friends (source_login,destination_login) VALUES('"+source_login+"','"+destination_login+"')";
            
            database::Database_MySQL::get().execute(query);

            query = "INSERT INTO friends(source_login,destination_login)  VALUES('"+destination_login+"','"+source_login+"')";
            
            database::Database_MySQL::get().execute(query);
        }

        void remove(){
            std::string query;

            query = "DELETE FROM friends WHERE source_login='"+source_login+"' and destination_login='"+destination_login+"'";
            database::Database_MySQL::get().execute(query);

            query = "DELETE FROM friends WHERE source_login='"+destination_login+"' and destination_login='"+source_login+"'";
            database::Database_MySQL::get().execute(query);
        }
    };
}


#endif