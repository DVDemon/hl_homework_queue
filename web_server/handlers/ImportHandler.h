    #ifndef IMPORTHANDLER_H
    #define IMPORTHANDLER_H

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
    #include "Poco/StreamCopier.h"
    #include "Poco/JSON/Parser.h"
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


    class ImportHandler: public HTTPRequestHandler
    {
    public:
        ImportHandler(const std::string& format): _format(format)
        {
        }

        void handleRequest(HTTPServerRequest& request,
                        HTTPServerResponse& response)
        {
            //Application& app = Application::instance();
            //app.logger().information("Login Request from " + request.clientAddress().toString());

            
            response.setChunkedTransferEncoding(true);
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
            response.setContentType("application/json");
            std::ostream& ostr = response.send();


            
            try{
                std::istream &is = request.stream();

                unsigned long long size = request.getContentLength();
                std::string buffer;
                buffer.reserve(size);
                for(unsigned long long i=0;i<size;++i){
                    char c;
                    is.read(&c,1);
                    buffer.push_back(c);
                }
                Poco::JSON::Parser parser;
                Poco::Dynamic::Var result = parser.parse(buffer);
                Poco::JSON::Array::Ptr subObject = result.extract<Poco::JSON::Array::Ptr>();
                long long i{};
                for (Poco::JSON::Array::ConstIterator it = subObject->begin(); it != subObject->end(); ++it)
                {
                    Poco::Dynamic::Var element = *it;
                    Poco::JSON::Object::Ptr object = element.extract<Poco::JSON::Object::Ptr>();
                    std::string id = object->getValue<std::string>("_id");
                    std::string age = object->getValue<std::string>("age");
                    std::string first_name = object->getValue<std::string>("first_name");
                    std::string last_name = object->getValue<std::string>("last_name");
                    std::string city = object->getValue<std::string>("city");

                    database::Person person;

                    person.login = id;
                    person.password_hash = webserver::Session::get().get_hash(id);
                    person.first_name  = first_name;
                    person.last_name   = last_name;
                    person.city        = city;
                    person.age         = atoi(age.c_str());

                    person.insert();

                    if((i%10000) == 0)
                    std::cout << "insert: " << (++i) << std::endl;
                    
                }
                //Poco::StreamCopier::copyStream(i, std::cout, request.getContentLength());
                ostr << "{ \"result\": \"true\" ,\"size\": \""+std::to_string(buffer.length())+"\"}";    
                }catch(...){
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
                }


        }

    private:
        std::string _format;
    };
    #endif // !IMPORTHANDLER_H