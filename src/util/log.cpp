#include "log.h"
#include <iostream>

/**
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log &mp::log::operator<<(char _msg)
{
    this->m_msg.append(1, _msg);
    return *this;
}

/**
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log &mp::log::operator<<(const char *_msg)
{
    this->m_msg.append(_msg);
    return *this;
}

/**
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log &mp::log::operator<<(std::string _msg)
{
    this->m_msg.append(_msg);
    return *this;
}

/**
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log &mp::log::operator<<(int32_t _msg)
{
    this->m_msg.append(std::to_string(_msg));
    return *this;
}

/**
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log &mp::log::operator<<(uint32_t _msg)
{
    this->m_msg.append(std::to_string(_msg));
    return *this;
}

/**
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log &mp::log::operator<<(int64_t _msg)
{
    this->m_msg.append(std::to_string(_msg));
    return *this;
}

/**
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log &mp::log::operator<<(uint64_t _msg)
{
    this->m_msg.append(std::to_string(_msg));
    return *this;
}

/**
** 填入日志内容，但不立即输出
** @param _msg 将要填入的内容 */
mp::log &mp::log::operator<<(double _msg)
{
    this->m_msg.append(std::to_string(_msg));
    return *this;
}

/**
** 日志操作
** @param _op 操作类型
** 若_op为push，则表示立即输出日志 */
mp::log &mp::log::operator<<(SOP _op)
{
    switch (_op)
    {
    case SOP::push:
        this->_push();
        break;

    case SOP::clear:
        this->_clear();
        break;

    default:
        break;
    }

    return *this;
}

/**
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

    std::cout << "[" << this->m_from << "] [" << this->m_title << "] ";
    std::cout << this->m_msg << std::endl;
    this->_clear();
}

/**
** 清空消息缓冲区消息内容 */
void mp::log::_clear()
{
    this->m_msg.resize(0);
}
