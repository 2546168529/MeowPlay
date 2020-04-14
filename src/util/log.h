#pragma once
#include <sstream>
#include <string>

namespace mp {
    
    class log {
        public:
        enum Type {
            info, wran, error
        };

        enum SOP {
            push
        };

        private:
        std::stringstream m_msg;
        std::string m_from;
        std::string m_title;
        log::Type m_type;

        public:
        log(const char* _from, const char* _title) 
            : m_from(_from), m_title(_title), m_type(info), m_msg() {};

        log(Type _type, const char* _from, const char* _title) 
            : m_from(_from), m_title(_title), m_type(_type), m_msg() {};

        void _push();

        log& operator<<(const char* _msg);
        log& operator<<(std::string _msg);
        log& operator<<(long long _msg);
        log& operator<<(unsigned long long _msg);
        log& operator<<(SOP _msg);
    };
    
}
