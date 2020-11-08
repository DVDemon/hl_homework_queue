        #ifndef SEARCHHANDLER_H
        #define SEARCHHANDLER_H

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

        class SearchHandler: public HTTPRequestHandler
        {
        public:
            SearchHandler(const std::string& format): _format(format)
            {
            }

            void handleRequest(HTTPServerRequest& request,
                            HTTPServerResponse& response)
            {
                //Application& app = Application::instance();
                //app.logger().information("Person Request from " + request.clientAddress().toString());


                HTMLForm form(request);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream& ostr = response.send();
                if(form.has("session_id"))
                if(form.has("first_name"))
                if(form.has("last_name"))
                {
                    std::string session_str = form.get("session_id"); 
                    std::string first_name  = form.get("first_name"); 
                    std::string last_name   = form.get("last_name"); 
                    
                    long  session_id = stol(session_str);

                    if(webserver::Session::get().is_correct(session_id)){
                        try{
                           
                            std::vector<database::Person> persons = database::Person::search(first_name,last_name);
                            
                            Poco::JSON::Array arr;
                            for(auto s: persons) arr.add(s.toJSON());
                            Poco::JSON::Stringifier::stringify(arr,ostr);           
                        }catch(...){
                            std::cout << "search query exception session_id:" << session_id << std::endl;
                        }
                    } 
                }
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
            }

        private:
            std::string _format;
        };
        #endif // !SEARCHHANDLER_H