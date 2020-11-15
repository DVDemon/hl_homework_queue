cmake_minimum_required(VERSION 3.2)

project(hl_homework_01 C CXX)

SET (WEBSERVICE_BINARY "server")

set(CMAKE_MODULE_PATH "/Users/dvdemon/OneDrive/Documents/Lectures/OOP/2020/Highload/hl_homework_queue/;${CMAKE_MODULE_PATH}")

find_package(OpenSSL)
find_package(GTest REQUIRED)
find_package(Threads)
find_package(ZLIB)
find_package(LibRDKafka)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})
include_directories("/usr/local/include/mysql")


link_directories("/usr/local/lib")

add_executable(${WEBSERVICE_BINARY} main.cpp 
                                    web_server/session/session.cpp
                                    web_server/config.cpp
                                    database/database_mysql.cpp
                                    web_server/kafka/kafka_producer.cpp)

target_include_directories(${WEBSERVICE_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${WEBSERVICE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )

target_link_libraries(${WEBSERVICE_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "libmysqlclient.a"
                             ZLIB::ZLIB
                             ${LibRDKafka_LIBRARIES})

set_target_properties(${WEBSERVICE_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${WEBSERVICE_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

install(TARGETS ${WEBSERVICE_BINARY} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT Dmitriy.Dzyuba@mts.ru)
include(CPack)

