#ifndef HTTPREQUESTFACTORY_H
#define HTTPREQUESTFACTORY_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
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

#include "handlers/TimeRequestHandler.h"
#include "handlers/WebPageHandler.h"
#include "handlers/PersonHandler.h"
#include "handlers/LoginHandler.h"
#include "handlers/SignupHandler.h"
#include "handlers/FriendsHandler.h"
#include "handlers/ImportHandler.h"
#include "handlers/SearchHandler.h"
#include "handlers/ConfigHandler.h"
#include "handlers/ChatHandler.h"

/*
static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}*/

static bool startsWith(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}


class HTTPRequestFactory: public HTTPRequestHandlerFactory
{
public:
    HTTPRequestFactory(const std::string& format):
        _format(format)
    {
    }

    HTTPRequestHandler* createRequestHandler(
        const HTTPServerRequest& request)
    {
        std::string html=".html"; 
        std::string person = "/person";
        std::string login = "/login";
        std::string signup = "/signup";
        std::string friends = "/friends";
        std::string import = "/import";
        std::string search = "/search";
        std::string config = "/config";
        std::string chat = "/chat";

        if (request.getURI() == "/")   return new TimeRequestHandler(_format);
        if (request.getURI().find(html)!=std::string::npos)     return new WebPageHandler(_format);
        if (startsWith(request.getURI(),person)) return new PersonHandler(_format);
        if (startsWith(request.getURI(),login))  return new LoginHandler(_format);
        if (startsWith(request.getURI(),signup))  return new SignupHandler(_format);
        if (startsWith(request.getURI(),friends))  return new FriendsHandler(_format);
        if (startsWith(request.getURI(),import))  return new ImportHandler(_format);
        if (startsWith(request.getURI(),search))  return new SearchHandler(_format);
        if (startsWith(request.getURI(),config))  return new ConfigHandler(_format);
        if (startsWith(request.getURI(),chat))  return new ChatHandler(_format);

        return 0;
    }

private:
    std::string _format;
};

#endif