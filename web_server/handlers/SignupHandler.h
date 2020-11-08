        #ifndef SIGNUPHANDLER_H
        #define SIGNUPHANDLER_H

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
        #include "../session/session.h"


        class SignupHandler: public HTTPRequestHandler
        {
        private:

            bool check_password(const std::string& password1, const std::string &password2,std::string &reason){
                if(password1!=password2) {
                        reason ="Password1 not equal Password 2";
                        return false;
                    }

                if(password1.length()<8){
                reason ="Password length shoud be at least 8 signs";
                return false; 
                }

                return true;    
            }

            bool check_login(const std::string& login,std::string &reason){
                if(login.length()<3) {
                    reason ="Login must be at leas 3 signs";
                    return false; 
                } 

                if(login.find(' ')!=std::string::npos){
                    reason ="Login can't contain spaces";
                    return false; 
                } 
                
                if(login.find('\t')!=std::string::npos){
                    reason ="Login can't contain spaces";
                    return false; 
                }   

                return true; 
            };

            bool check_first_name(const std::string& first_name,std::string &reason){
                if(first_name.length()<3) {
                    reason ="First name must be at leas 3 signs";
                    return false; 
                } 

                if(first_name.find(' ')!=std::string::npos){
                    reason ="First name can't contain spaces";
                    return false; 
                } 
                
                if(first_name.find('\t')!=std::string::npos){
                    reason ="First name can't contain spaces";
                    return false; 
                }   

                return true; 
            };

            bool check_last_name(const std::string& last_name,std::string &reason){
                if(last_name.length()<3) {
                    reason ="Last name must be at leas 3 signs";
                    return false; 
                } 

                if(last_name.find(' ')!=std::string::npos){
                    reason ="Last name can't contain spaces";
                    return false; 
                } 
                
                if(last_name.find('\t')!=std::string::npos){
                    reason ="Last name can't contain spaces";
                    return false; 
                }   

                return true; 
            };


            bool check_age(const std::string& age,std::string &reason){
                int age_n = atoi(age.c_str());

                if(age_n<=0){
                    reason ="Age must be possitive number";
                    return false; 
                }   

                return true; 
            };
        public:
            SignupHandler(const std::string& format): _format(format)
            {
            }

            void handleRequest(HTTPServerRequest& request,
                            HTTPServerResponse& response)
            {
                //Application& app = Application::instance();
                //app.logger().information("Signup Request from " + request.clientAddress().toString());

                HTMLForm form(request,request.stream());
                response.setChunkedTransferEncoding(true);
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                response.setContentType("application/json");
                std::ostream& ostr = response.send();

                if(form.has("login"))
                if(form.has("password1"))
                if(form.has("password2"))
                if(form.has("first_name"))
                if(form.has("last_name"))
                if(form.has("age"))
                if(form.has("hobby"))
                if(form.has("city"))
                {

                    std::string login=form.get("login"); 
                    std::string password1=form.get("password1"); 
                    std::string password2=form.get("password2"); 
                    std::string first_name=form.get("first_name"); 
                    std::string last_name=form.get("last_name"); 
                    std::string age=form.get("age");
                    std::string hobby=form.get("hobby");
                    std::string city=form.get("city");
        
                    bool check_result = true;
                    std::string message;
                    std::string reason;

                    if(!check_password(password1,password2,reason)){
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }

                    if(!check_login(login,reason)){
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }

                    if(!check_first_name(first_name,reason)){
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }


                    if(!check_last_name(last_name,reason)){
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }


                    if(!check_age(age,reason)){
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }

                    if(database::Person::check_exist(login)){
                        check_result = false;
                        message += "login already exist";
                        message += "<br>";
                    }
                    
                    if(check_result){
                        try{
                        database::Person person;
                        person.login = login;
                        person.password_hash = webserver::Session::get().get_hash(password1);
                        person.first_name = first_name;
                        person.last_name  = last_name;
                        person.age = atoi(age.c_str());
                        person.hobby = hobby;
                        person.city = city;
                        person.insert();
                        ostr << "{ \"result\": true }";
                        return;
                    }catch(...){
                        ostr << "{ \"result\": false , \"reason\": \" database error\" }";
                    }

                    } else
                    {
                        
                        
                        ostr << "{ \"result\": false , \"reason\": \"" << message << "\" }";
                        return;
                    }
                    
                } 
                
                ostr << "{ \"result\": false , \"reason\": \"request broken\" }";

            }

        private:
            std::string _format;
        };
        #endif // !SIGNUPHANDLER_H