    #ifndef LOGINHANDLER_H
    #define LOGINHANDLER_H

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


    class LoginHandler: public HTTPRequestHandler
    {
    public:
        LoginHandler(const std::string& format): _format(format)
        {
        }

        void handleRequest(HTTPServerRequest& request,
                        HTTPServerResponse& response)
        {
            //Application& app = Application::instance();
            //app.logger().information("Login Request from " + request.clientAddress().toString());

            HTMLForm form(request);
            response.setChunkedTransferEncoding(true);
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
            response.setContentType("application/json");
            std::ostream& ostr = response.send();


            if(form.has("login"))
            if(form.has("password")){
                std::string login=form.get("login"); 
                std::string password=form.get("password"); 
                std::string password_hash = webserver::Session::get().get_hash(password);
                try{
                    database::Person person = database::Person::get_person(login); 
                    if(password_hash == person.password_hash){
                            long session_id = webserver::Session::get().get_session(person.login);
                            ostr << "{ \"result\": true ,";
                            ostr << "\"session_id\":" <<std::to_string(session_id) <<"}";
                            return;
                    }          
                }catch(...){
                    //response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
                }
            } 
            
            ostr << "{ \"result\": false }";

        }

    private:
        std::string _format;
    };
    #endif // !LOGINHANDLER_H