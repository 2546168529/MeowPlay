#pragma once
#include <sstream>
#include <string>

namespace mplog {

    enum Op {
        push
    };

    enum Type {
        INFO, WRAN, ERROR
    };

    class message {
        private:
        std::stringstream m_msg;
        std::string m_from;
        std::string m_title;
        mplog::Type m_type;

        public:
        message(const char* _from, const char* _title) 
            : m_from(_from), m_title(_title), m_type(mplog::INFO), m_msg() {};

        message(Type _type, const char* _from, const char* _title) 
            : m_from(_from), m_title(_title), m_type(_type), m_msg() {};

        void push();

        message& operator<<(const char* _msg);
        message& operator<<(std::string _msg);
        message& operator<<(long long _msg);
        message& operator<<(unsigned long long _msg);
        message& operator<<(Op _msg);
    };
    
}
