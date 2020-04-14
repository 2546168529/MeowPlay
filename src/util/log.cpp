#include "log.h"

#include <iostream>

/*
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log& mp::log::operator<<(const char* _msg)
{
    m_msg << _msg;
    return *this;
}

/*
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log& mp::log::operator<<(std::string _msg)
{
    m_msg << _msg;
    return *this;
}

/*
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log& mp::log::operator<<(long long _msg)
{
    m_msg << _msg;
    return *this;
}

/*
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log& mp::log::operator<<(unsigned long long _msg)
{
    m_msg << _msg;
    return *this;
}

/*
** 日志操作
** @param _op 操作类型
** 若_op为push，则表示立即输出日志 */
mp::log& mp::log::operator<<(SOP _op)
{
    switch (_op)
    {
    case SOP::push:
        _push();
        break;
    
    default:
        break;
    }
    
    return *this;
}

/*
** 输出日志
** 在信息填入完毕后调用此函数输出填入的内容 */
void mp::log::_push()
{
    switch (m_type)
    {
    case info:
        std::cout << "[info] ";
        break;
    
    case wran:
        std::cout << "[wran] ";
        break;
    
    case error:
        std::cout << "[error] ";
        break;
    
    default:
        break;
    }

    std::cout << "["  << m_from << "] [" << m_title << "] ";
    std::cout << m_msg.str() << std::endl;
    m_msg.clear();
    
}