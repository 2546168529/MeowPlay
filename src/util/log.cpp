#include "log.h"
#include <iostream>

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(char _msg)
{
    this->m_msg.append(1, _msg);
    return *this;
}

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(const char* _msg)
{
    this->m_msg.append(_msg);
    return *this;
}

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(std::string _msg)
{
    this->m_msg.append(_msg);
    return *this;
}

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(int64_t _msg)
{
    this->m_msg.append(std::to_string(_msg));
    return *this;
}

/*
** ������־���ݣ������������
** @param _msg ��Ҫ��������� */
mp::log& mp::log::operator<<(double _msg)
{
    this->m_msg.append(std::to_string(_msg));
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

    std::cout << "["  << this->m_from << "] [" << this->m_title << "] ";
    std::cout << this->m_msg << std::endl;
    this->_clear();
    
}

/*
** �����Ϣ��������Ϣ���� */
void mp::log::_clear()
{
    this->m_msg.resize(0);
}
