#pragma once
#include <string>

namespace mp {
    
    class log {
    public:
        
        enum Type {
            info, wran, error
        };

        enum SOP {
            push,
            clear
        };

    private:
        std::string m_msg;
        std::string m_from;
        std::string m_title;
        log::Type m_type;

    public:
        log(const char* _from, const char* _title) 
            : m_from(_from), m_title(_title), m_type(info), m_msg() {};

        log(Type _type, const char* _from, const char* _title) 
            : m_from(_from), m_title(_title), m_type(_type), m_msg() {};

        void _push();
        void _clear();

        log& operator<<(char _msg);
        log& operator<<(const char* _msg);
        log& operator<<(std::string _msg);
        log& operator<<(int64_t _msg);
        log& operator<<(uint64_t _msg);
        log& operator<<(int32_t _msg);
        log& operator<<(uint32_t _msg);
        log& operator<<(double _msg);
        log& operator<<(SOP _op);
    };
    
}
