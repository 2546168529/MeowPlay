#include "database.h"
#include <iostream>

mp::database::stmt::stmt(sqlite3* _connect, const char* sql)
	: m_use(new size_t(1))
{
	const char* tail;
	int rc = sqlite3_prepare_v2(_connect, sql, -1, &this->m_stmt, &tail);
	this->m_last_error = rc;
	m_open_success = (rc == SQLITE_OK);

}

mp::database::stmt::stmt(const stmt& _stmt) noexcept
{
	m_use = _stmt.m_use;
	(*this->m_use)++;

	this->m_open_success = _stmt.m_open_success;
	this->m_last_error = _stmt.m_last_error;
	this->m_stmt = _stmt.m_stmt;
}

mp::database::stmt& mp::database::stmt::operator=(const stmt& _stmt) noexcept
{
	++* _stmt.m_use;//�Ƚ������stmt�����üǴ�+1�������Ը�ֵ��ɵ��쳣

	free();

	this->m_use = _stmt.m_use;
	this->m_open_success = _stmt.m_open_success;
	this->m_last_error = _stmt.m_last_error;
	this->m_stmt = _stmt.m_stmt;
}

mp::database::stmt::~stmt()
{
	this->free();
}

mp::database::stmt& mp::database::stmt::operator=(stmt&& _stmt) noexcept
{
	if (this != &_stmt)
	{
		this->free();

		this->m_use = _stmt.m_use;
		this->m_open_success = _stmt.m_open_success;
		this->m_last_error = _stmt.m_last_error;
		this->m_stmt = _stmt.m_stmt;

		_stmt.m_use = nullptr;
		_stmt.m_stmt = nullptr;
	}
	return *this;
}

mp::database::stmt::stmt(stmt&& _stmt) noexcept
	: m_stmt(_stmt.m_stmt), m_use(_stmt.m_use), m_last_error(_stmt.m_last_error), m_open_success(_stmt.m_open_success)
{
	_stmt.m_use = nullptr;
	_stmt.m_stmt = nullptr;
}

/**
** �ͷ���Դ 
** ÿ�ε��ã�use�����ݼ�1����useΪ0�����ͷ���Դ
*/
void mp::database::stmt::free()
{
	if (this->m_use != nullptr && -- * (this->m_use) == 0)
	{
		delete this->m_use;
		this->finalize();
		this->m_use = nullptr;	
	}
}

/**
** �ͷ�stmt
** @return ����stmt�Ƿ�ɹ��ͷ� 
*/
bool mp::database::stmt::finalize()
{
	int rc = sqlite3_finalize(this->m_stmt);
	if (rc == SQLITE_OK)
	{
		//���ͷųɹ�����stmtָ��Ϊnullptr
		this->m_stmt = nullptr;
		return true;
	}
	else
	{
		//���ͷ�ʧ�ܣ���¼�������
		this->m_last_error = rc;
		return false;
	}
}

/**
** ����ִ��
** @return ����ִ��״̬
*/
int mp::database::stmt::step()
{
	return sqlite3_step(this->m_stmt);
}

/**
** ����stmt
** @return �ɹ�����true��������ʧ�ܣ�����false����ͨ��last_error��ô������
*/
bool mp::database::stmt::reset()
{
	int rc = sqlite3_reset(this->m_stmt);
	if (rc != SQLITE_OK)
	{
		this->m_last_error = rc;
		return false;
	}
	return true;
}

mp::database::manage::manage(const manage& _manage) noexcept
{
	++* _manage.m_use;
	this->m_use = _manage.m_use;
	this->m_last_error = _manage.m_last_error;
	this->m_connect = _manage.m_connect;
}

mp::database::manage::manage(manage&& _manage) noexcept
	: m_use(_manage.m_use), m_connect(_manage.m_connect), m_last_error(_manage.m_last_error)
{
	_manage.m_use = nullptr;
	_manage.m_connect = nullptr;
}

mp::database::manage& mp::database::manage::operator=(const manage& _manage) noexcept
{
	++* _manage.m_use;
	this->free();
	this->m_use = _manage.m_use;
	this->m_last_error = _manage.m_last_error;
	this->m_connect = _manage.m_connect;
}

mp::database::manage& mp::database::manage::operator=(manage&& _manage) noexcept
{

	if(this != &_manage)
	{
		this->free();

		this->m_use = _manage.m_use;
		this->m_last_error = _manage.m_last_error;
		this->m_connect = _manage.m_connect;

		_manage.m_use = nullptr;
		_manage.m_connect = nullptr;
	}

	return *this;
}

mp::database::manage::~manage()
{
	this->free();
}

/**
** �����ݿ�
** @param _filename ��Ҫ�򿪵����ݿ��ļ�·��
** @param _flag ���ݿ�򿪷�ʽ
** @return �Ƿ�ɹ������ݿ�
*/
bool mp::database::manage::open(const char* _filename, int _flag)
{
	int rc = sqlite3_open_v2(_filename, &this->m_connect, _flag, nullptr);
	if (!(rc == SQLITE_OK))
	{
		m_last_error = rc;
		return false;
	}			
	return true;
}

/**
** �ͷ���Դ 
** ÿ�ε��ã�use�����ݼ�1����useΪ0�����ͷ���Դ
*/
void mp::database::manage::free()
{
	if (this->m_use != nullptr && -- * (this->m_use) == 0)
	{
		delete this->m_use;
		this->close();
	}	
}

/**
** ִ�зǲ�ѯ���
** @param _sql sql��䣨�����ô˷���ִ�в�ѯ��䣩
** @return �ɹ�����true����ִ��ʧ�ܣ�����false����ͨ��last_error��ô������
*/
bool mp::database::manage::exec_noquery(const char* _sql)
{
	char* pchar_err_message = nullptr;
	int rc = sqlite3_exec(this->m_connect,
		_sql,
		[](void* data, int argc, char** argv, char** azColName) -> int { return SQLITE_OK; },
		nullptr,
		&pchar_err_message);
	if (rc != SQLITE_OK)
	{
		this->m_last_error = rc;
	}

	return rc == SQLITE_OK;
}

/**
** ׼��sql���
** @param _sql sql���
** @return stmt����ͨ��stmt.open_success()���ж�sql�Ƿ�׼���ɹ�
*/
mp::database::stmt mp::database::manage::prepare(const char* _sql)
{
	return stmt(this->m_connect, _sql);
}

/**
** �رյ�ǰ��������ݿ�
** @return �ɹ�����true����ִ��ʧ�ܣ�����false����ͨ��last_error��ô������
*/
bool mp::database::manage::close()
{
	int rc = sqlite3_close_v2(this->m_connect);
	if (rc != SQLITE_OK)
	{
		this->m_last_error = rc;
		return false;
	}
	else
	{
		this->m_connect = nullptr;
		return true;
	}
}

/**
** ��ȡ���Ĵ������
** @return �������
*/
int mp::database::manage::last_error()
{
	return this->m_last_error;
}

/**
** ��ȡ������Ϣ
** @return ������Ϣ�������ͷ�ָ�룩
*/
const char* mp::database::manage::errmsg()
{
	return sqlite3_errmsg(this->m_connect);
}

/**
** ��ȡ���Ĵ�������
** @return ���������������ͷ�ָ�룩
*/
const char* mp::database::manage::errstr()
{
	return sqlite3_errstr(this->m_last_error);
}

/**
** ȡָ���������Ĵ�������
** @param _rc �������
** @return ���������������ͷ�ָ�룩
*/
const char* mp::database::manage::errstr(int _rc)
{
	return sqlite3_errstr(_rc);
}

/**
** ��ȡ��ǰ��������ݿ�����ָ��
** @return sqlite3ָ��
*/
sqlite3* mp::database::manage::connect_ptr()
{
	return this->m_connect;
}

/**
** ���õ�ǰ��������ݿ�����ָ��
** @param _connect ��Ҫ���������ݿ�ָ��
** @return �ɵ����ݿ�ָ��
*/
sqlite3* mp::database::manage::connect_ptr(sqlite3* _connect)
{
	sqlite3* bak = this->m_connect;
	this->m_connect = _connect;
	return bak;
}

