#include "log.h"
#include <iostream>

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(char _msg)
{
    m_msg.append(1, _msg);
    return *this;
}

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(const char* _msg)
{
    m_msg.append(_msg);
    return *this;
}

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(std::string _msg)
{
    m_msg.append(_msg);
    return *this;
}

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(int64_t _msg)
{
    m_msg.append(std::to_string(_msg));
    return *this;
}

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(double _msg)
{
    m_msg.append(std::to_string(_msg));
    return *this;
}

/*
** ��־����
** @param _op ��������
** ��_opΪpush�����ʾ���������־ */
mp::log& mp::log::operator<<(SOP _op)
{
    switch (_op)
    {
    case SOP::push:
        _push();
        break;
    
    case SOP::clear:
        _clear();
        break;

    default:
        break;
    }
    
    return *this;
}

/*
** �����־
** ����Ϣ������Ϻ���ô˺��������������� */
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
    std::cout << m_msg << std::endl;
    _clear();
    
}

/*
** �����Ϣ��������Ϣ���� */
void mp::log::_clear()
{
    m_msg.resize(0);
}
