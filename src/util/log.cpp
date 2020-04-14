#include "log.h"

#include <iostream>

mp::log& mp::log::operator<<(const char* _msg)
{
    m_msg << _msg;
    return *this;
}

mp::log& mp::log::operator<<(std::string _msg)
{
    m_msg << _msg;
    return *this;
}

mp::log& mp::log::operator<<(long long _msg)
{
    m_msg << _msg;
    return *this;
}

mp::log& mp::log::operator<<(unsigned long long _msg)
{
    m_msg << _msg;
    return *this;
}

mp::log& mp::log::operator<<(SOP _msg)
{
    switch (_msg)
    {
    case SOP::push:
        _push();
        break;
    
    default:
        break;
    }
    
    return *this;
}

void mp::log::_push()
{

    std::cout << m_msg.str() << std::endl;
    m_msg.clear();

}