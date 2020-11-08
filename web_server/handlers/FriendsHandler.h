        #ifndef FRIENDSHANDLER_H
        #define FRIENDSHANDLER_H

        #include "Poco/Net/HTTPServer.h"
        #include "Poco/Net/HTTPRequestHandler.h"
        #include "Poco/Net/HTTPRequestHandlerFactory.h"
        #include "Poco/Net/HTTPServerParams.h"
        #include "Poco/Net/HTTPServerRequest.h"
        #include "Poco/Net/HTTPServerResponse.h"
        #include "Poco/Net/HTTPServerParams.h"
        #include "Poco/Net/HTMLForm.h"
        #include "Poco/Net/ServerSocket.h"
        #include "Poco/Timestamp.h"
        #include "Poco/DateTimeFormatter.h"
        #include "Poco/DateTimeFormat.h"
        #include "Poco/Exception.h"
        #include "Poco/ThreadPool.h"
        #include "Poco/Util/ServerApplication.h"
        #include "Poco/Util/Option.h"
        #include "Poco/Util/OptionSet.h"
        #include "Poco/Util/HelpFormatter.h"
        #include <iostream>
        #include <iostream>
        #include <fstream>

        using Poco::Net::ServerSocket;
        using Poco::Net::HTTPRequestHandler;
        using Poco::Net::HTTPRequestHandlerFactory;
        using Poco::Net::HTTPServer;
        using Poco::Net::HTTPServerRequest;
        using Poco::Net::HTTPServerResponse;
        using Poco::Net::HTTPServerParams;
        using Poco::Timestamp;
        using Poco::DateTimeFormatter;
        using Poco::DateTimeFormat;
        using Poco::ThreadPool;
        using Poco::Util::ServerApplication;
        using Poco::Util::Application;
        using Poco::Util::Option;
        using Poco::Util::OptionSet;
        using Poco::Util::OptionCallback;
        using Poco::Util::HelpFormatter;
        using Poco::Net::NameValueCollection;
        using Poco::Net::HTMLForm;

        #include "../../database/person.h"
        #include "../../database/friends.h"
        #include "../session/session.h"

        #include <map>

        class FriendsHandler: public HTTPRequestHandler
        {
        public:
            FriendsHandler(const std::string& format): _format(format)
            {
            }

            void handleRequest(HTTPServerRequest& request,
                            HTTPServerResponse& response)
            {
                //Application& app = Application::instance();
               // app.logger().information("Friends Request from " + request.clientAddress().toString());


                HTMLForm form(request);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream& ostr = response.send();
                if(form.has("session_id") && !form.has("filter") && !form.has("add") && !form.has("delete")){
                    std::string session_str=form.get("session_id"); 
                    
                    long  session_id = stol(session_str);

                    if(webserver::Session::get().is_correct(session_id)){
                        try{
                            std::string login = webserver::Session::get().get_login(session_id);
                            std::vector<database::Friends> friends = database::Friends::get_friends(login); 
                            Poco::JSON::Array arr;
                            for(auto s: friends) arr.add(s.destination_login);         
                            Poco::JSON::Stringifier::stringify(arr,ostr);
                        }catch(...){
                        }
                    } 
                } else
                if(form.has("session_id")&&(form.has("filter"))){
                    std::string session_str=form.get("session_id"); 
                    std::string filter=form.get("filter");
                    
                    long  session_id = stol(session_str);

                    if(webserver::Session::get().is_correct(session_id)){
                        try{
                            std::vector<std::string> logins = database::Person::get_logins_starting(filter); 
                            Poco::JSON::Array arr;

                            for(auto s: logins) arr.add(s);
                                     
                            Poco::JSON::Stringifier::stringify(arr,ostr);
                        }catch(...){
                            std::cout << "exception:" << session_id << std::endl;
                        }
                    } 
                } else
                if(form.has("session_id")&&(form.has("add"))){
                    std::string session_str=form.get("session_id"); 
                    std::string friend_login=form.get("add");
                    
                    long  session_id = stol(session_str);

                    if(webserver::Session::get().is_correct(session_id)){
                        try{
                            std::string login = webserver::Session::get().get_login(session_id);
                            database::Person person = database::Person::get_person(login);
                            database::Person friend_person = database::Person::get_person(friend_login);

                            if(person.login!=friend_person.login){
                                database::Friends friends {person.login, friend_person.login};
                                friends.remove();
                                friends.insert();
                                     
                            ostr << "{ 'result': 'ok'}" << std::endl;
                            } else ostr << "{ 'result': 'false'}" << std::endl;
                        }catch(...){
                            std::cout << "exception:" << session_id << std::endl;
                        }
                    } 
                } else
                if(form.has("session_id")&&(form.has("delete"))){
                    std::string session_str=form.get("session_id"); 
                    std::string friend_login=form.get("delete");
                    
                    long  session_id = stol(session_str);

                    if(webserver::Session::get().is_correct(session_id)){
                        try{
                            std::string login = webserver::Session::get().get_login(session_id);
                            database::Person person = database::Person::get_person(login);
                            database::Person friend_person = database::Person::get_person(friend_login);

                            if(person.login!=friend_person.login){
                                database::Friends friends {person.login, friend_person.login};
                                friends.remove();
                                ostr << "{ 'result': 'ok'}" << std::endl;
                            } else ostr << "{ 'result': 'false'}" << std::endl;
                        }catch(...){
                            std::cout << "exception:" << session_id << std::endl;
                        }
                    } 
                }
               
                
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
            }

        private:
            std::string _format;
        };
        #endif // !FRIENDSHANDLER_H