#include <iostream>
#include <string>

#include "web_server/httpwebserver.h"






int main(int argc, char *argv[])
{
    std::cout << "MySQL version:" << database::Database_MySQL::get_version() << std::endl;
    database::Database_MySQL::get();


    HTTPWebServer app;
    return app.run(argc, argv);
}