#ifndef PERSON_H
#define PERSON_H

#include "Poco/JSON/Object.h"
#include <string>
#include <vector>
#include "database_mysql.h"

namespace database{
    struct Person{
            std::string login;
            std::string password_hash;

            std::string first_name;
            std::string last_name;
            int         age;
            std::string hobby;
            std::string city;

            Poco::JSON::Object::Ptr toJSON() const{
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

                root->set("login",login);
                root->set("password_hash",password_hash);
                root->set("first_name",first_name);
                root->set("last_name",last_name);
                root->set("age",age);
                root->set("hobby",hobby);
                root->set("city",city);
                return root;
            }

            static void init(){
                try{
                    std::cout << "initializing person ..." << std::endl;
                    std::string query = "CREATE TABLE person (`login` varchar(256) NOT NULL,`password_hash` varchar(1024) NOT NULL,`first_name` varchar(1024) DEFAULT NULL,`last_name` varchar(1024) DEFAULT NULL,`age` int DEFAULT NULL,`hobby` varchar(4096) DEFAULT NULL,`city` varchar(1024) DEFAULT NULL, PRIMARY KEY (`login`))";
                    database::Database_MySQL::get().execute(query);
                    std::cout << "initializing person ... done" << std::endl;
                }catch(...){
                    std::cout << "initializing person ... fail" << std::endl;
                }

            }

            static std::vector<std::string> get_logins_starting(const std::string& filter){
                std::vector<std::string> result;
                std::string record;

                std::string query="SELECT login FROM person WHERE login LIKE'"+filter+"%'";
                database::Database_MySQL::get().query(query,[&](int ,int column,std::string value)
                {

                    switch(column){
                            case 0: record = value; 
                            break;
                            
                        }

                    
                },[&](int){
                    result.push_back(record);
                });
                
                return result;
            }

            static bool   check_exist(std::string & login){
                int count{-1};

                std::string query="SELECT count(*) FROM person WHERE login='"+login+"'";
                database::Database_MySQL::get().query(query,[&](int row,int column,std::string value)
                {
                    if ((row ==0)&&(column==0)) count = atoi(value.c_str());
                },[](int){});

                if(count<0) throw std::logic_error("error executing query");
                return (count>0) ;
            }

            static Person get_person(std::string & match){
                Person person;
                bool found{};


                std::string query="SELECT login, password_hash, first_name,last_name,age,hobby,city FROM person WHERE login='"+match+"'";
                database::Database_MySQL::get().query(query,[&](int row,int column,std::string value)
                {
                    found = true;
                    if (row ==0)
                        switch(column){
                            case 0: person.login = value; break;
                            case 1: person.password_hash= value;break;
                            case 2: person.first_name=value;break;
                            case 3: person.last_name=value;break;
                            case 4: person.age=atoi(value.c_str());break;
                            case 5: person.hobby=value;break;
                            case 6: person.city=value;break;
                        }
                    
                },[](int){});

                if(found) return person;

                throw std::logic_error("not found");
            }

            static std::vector<Person> get_persons(){
                std::vector<Person> result;
                Person person;

                std::string query="SELECT login, password_hash, first_name,last_name,age,hobby,city FROM person";
                database::Database_MySQL::get().query(query,[&](int ,int column,std::string value)
                {
                        switch(column){
                            case 0: person.login = value; break;
                            case 1: person.password_hash= value;break;
                            case 2: person.first_name=value;break;
                            case 3: person.last_name=value;break;
                            case 4: person.age=atoi(value.c_str());break;
                            case 5: person.hobby=value;break;
                            case 6: person.city=value;break;
                        }
                    
                },[&](int){
                    result.push_back(person);
                });
                

                return result;
            }

            static std::vector<Person> search(const std::string& first_name, const std::string& last_name){
                std::vector<Person> result;
                Person person;

                std::string query="SELECT login, password_hash, first_name,last_name,age,hobby,city FROM person WHERE ";
                query += "first_name LIKE '";
                query += first_name;
                query += "%' and last_name LIKE '";
                query += last_name;
                query += "%'";

                
                database::Database_MySQL::get().query(query,[&](int ,int column,std::string value)
                {
                        switch(column){
                            case 0: person.login = value; break;
                            case 1: person.password_hash= value;break;
                            case 2: person.first_name=value;break;
                            case 3: person.last_name=value;break;
                            case 4: person.age=atoi(value.c_str());break;
                            case 5: person.hobby=value;break;
                            case 6: person.city=value;break;
                        }
                    
                },[&](int){
                    result.push_back(person);
                });
                

                return result;
            }

            static std::vector<Person> get_friends(){
                std::vector<Person> result;
                Person person;

                std::string query="SELECT login, password_hash, first_name,last_name,age,hobby,city FROM person";
                database::Database_MySQL::get().query(query,[&](int row,int column,std::string value)
                {
                    if (row ==0)
                        switch(column){
                            case 0: person.login = value; break;
                            case 1: person.password_hash= value;break;
                            case 2: person.first_name=value;break;
                            case 3: person.last_name=value;break;
                            case 4: person.age=atoi(value.c_str());break;
                            case 5: person.hobby=value;break;
                            case 6: person.city=value;break;
                        }
                    
                },[&](int){
                    result.push_back(person);
                });
                

                return result;
            }

            void insert(){
                std::string query="INSERT INTO person VALUES(";
                query+="'"+login+"',";
                query+="'"+password_hash+"',";
                query+="'"+first_name+"',";
                query+="'"+last_name+"',";
                query+="'"+std::to_string(age)+"',";
                query+="'"+hobby+"',";
                query+="'"+city+"'";
                query+=")";
                database::Database_MySQL::get().execute(query);
            }
    };
}
#endif