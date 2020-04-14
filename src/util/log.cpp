#include "log.h"

#include <iostream>

mplog::message& mplog::message::operator<<(const char* _msg)
{
    m_msg << _msg;
    return *this;
}

mplog::message& mplog::message::operator<<(std::string _msg)
{
    m_msg << _msg;
    return *this;
}

mplog::message& mplog::message::operator<<(long long _msg)
{
    m_msg << _msg;
    return *this;
}

mplog::message& mplog::message::operator<<(unsigned long long _msg)
{
    m_msg << _msg;
    return *this;
}

mplog::message& mplog::message::operator<<(Op _msg)
{
    switch (_msg)
    {
    case Op::push:
        push();
        break;
    
    default:
        break;
    }
    
    return *this;
}

void mplog::message::push()
{

    std::cout << m_msg.str() << std::endl;
    m_msg.clear();

}