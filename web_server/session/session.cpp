#include "session.h"

#include <iostream>

#include "Poco/HMACEngine.h"
#include "Poco/SHA1Engine.h"

using Poco::DigestEngine;
using Poco::SHA1Engine;
using Poco::HMACEngine;

namespace webserver{
Session::Session(){

}

Session &Session::get(){
    static Session instance;
    return instance;
}

std::string Session::get_hash(const std::string & value){
    HMACEngine<SHA1Engine> hmac(value); // we'll compute a HMAC-SHA1
    const DigestEngine::Digest& digest = hmac.digest();
        
    return std::string(DigestEngine::digestToHex(digest));
}

long Session::get_session(const std::string& login){
    std::lock_guard<std::mutex> lck(_mtx);
    static long session_id = 0;

    ++session_id;
//    _sessions.insert(std::pair<long,std::string>(session_id,login));
    _sessions[session_id] = login;
    return session_id;
}
bool Session::is_correct(long session_id){
    std::lock_guard<std::mutex> lck(_mtx);
    return  _sessions.find(session_id)!=_sessions.end();
}
std::string Session::get_login(long session_id){
    std::lock_guard<std::mutex> lck(_mtx);
    return _sessions[session_id];
}

}