#ifndef SESSION_H
#define SESSION_H



#include <map>
#include <string>
#include <mutex>

namespace webserver{
    class Session{
        private:
            Session();
            std::mutex                 _mtx;
            std::map<long,std::string> _sessions;
        public:
            static Session& get();    
            std::string get_hash(const std::string & value);
            long get_session(const std::string& login);
            bool is_correct(long session_id);
            std::string get_login(long session_id);
    };
}

#endif // !SESSION_H
