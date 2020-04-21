#include "database.h"
#include <iostream>

mp::database::stmt::stmt(sqlite3 *_connect, const char *sql)
	: m_use(new size_t(1))
{
	const char *tail;
	int rc = sqlite3_prepare_v2(_connect, sql, -1, &this->m_stmt, &tail);
	this->m_last_error = rc;
	m_open_success = (rc == SQLITE_OK);
}

mp::database::stmt::stmt(const stmt &_stmt) noexcept
{
	m_use = _stmt.m_use;
	(*this->m_use)++;

	this->m_open_success = _stmt.m_open_success;
	this->m_last_error = _stmt.m_last_error;
	this->m_stmt = _stmt.m_stmt;
}

mp::database::stmt &mp::database::stmt::operator=(const stmt &_stmt) noexcept
{
	++*_stmt.m_use; //先将传入的stmt的所用记次+1，避免自赋值造成的异常

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

mp::database::stmt &mp::database::stmt::operator=(stmt &&_stmt) noexcept
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

mp::database::stmt::stmt(stmt &&_stmt) noexcept
	: m_stmt(_stmt.m_stmt), m_use(_stmt.m_use), m_last_error(_stmt.m_last_error), m_open_success(_stmt.m_open_success)
{
	_stmt.m_use = nullptr;
	_stmt.m_stmt = nullptr;
}

/**
** 释放资源 
** 每次调用，use计数递减1，若use为0，则释放资源
*/
void mp::database::stmt::free()
{
	if (this->m_use != nullptr && --*(this->m_use) == 0)
	{
		delete this->m_use;
		this->finalize();
		this->m_use = nullptr;
	}
}

/**
** 释放stmt
** @return 返回stmt是否成功释放 
*/
bool mp::database::stmt::finalize()
{
	int rc = sqlite3_finalize(this->m_stmt);
	if (rc == SQLITE_OK)
	{
		//若释放成功，置stmt指针为nullptr
		this->m_stmt = nullptr;
		return true;
	}
	else
	{
		//若释放失败，记录错误代码
		this->m_last_error = rc;
		return false;
	}
}

/**
** 单步执行
** @return 返回执行状态
*/
int mp::database::stmt::step()
{
	return sqlite3_step(this->m_stmt);
}

/**
** 重置stmt
** @return 成功返回true，若重置失败，返回false，可通过last_error获得错误代码
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

mp::database::manage::manage(const manage &_manage) noexcept
{
	++*_manage.m_use;
	this->m_use = _manage.m_use;
	this->m_last_error = _manage.m_last_error;
	this->m_connect = _manage.m_connect;
}

mp::database::manage::manage(manage &&_manage) noexcept
	: m_use(_manage.m_use), m_connect(_manage.m_connect), m_last_error(_manage.m_last_error)
{
	_manage.m_use = nullptr;
	_manage.m_connect = nullptr;
}

mp::database::manage &mp::database::manage::operator=(const manage &_manage) noexcept
{
	++*_manage.m_use;
	this->free();
	this->m_use = _manage.m_use;
	this->m_last_error = _manage.m_last_error;
	this->m_connect = _manage.m_connect;
}

mp::database::manage &mp::database::manage::operator=(manage &&_manage) noexcept
{

	if (this != &_manage)
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
** 打开数据库
** @param _filename 将要打开的数据库文件路径
** @param _flag 数据库打开方式
** @return 是否成功打开数据库
*/
bool mp::database::manage::open(const char *_filename, int _flag)
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
** 释放资源 
** 每次调用，use计数递减1，若use为0，则释放资源
*/
void mp::database::manage::free()
{
	if (this->m_use != nullptr && --*(this->m_use) == 0)
	{
		delete this->m_use;
		this->close();
	}
}

/**
** 执行非查询语句
** @param _sql sql语句（不能用此方法执行查询语句）
** @return 成功返回true，若执行失败，返回false，可通过last_error获得错误代码
*/
bool mp::database::manage::exec_noquery(const char *_sql)
{
	char *pchar_err_message = nullptr;
	int rc = sqlite3_exec(
		this->m_connect,
		_sql,
		[](void *data, int argc, char **argv, char **azColName) -> int { return SQLITE_OK; },
		nullptr,
		&pchar_err_message);
	if (rc != SQLITE_OK)
	{
		this->m_last_error = rc;
	}

	return rc == SQLITE_OK;
}

/**
** 准备sql语句
** @param _sql sql语句
** @return stmt，可通过stmt.open_success()来判断sql是否准备成功
*/
mp::database::stmt mp::database::manage::prepare(const char *_sql)
{
	return stmt(this->m_connect, _sql);
}

/**
** 关闭当前管理的数据库
** @return 成功返回true，若执行失败，返回false，可通过last_error获得错误代码
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
** 获取最后的错误代码
** @return 错误代码
*/
int mp::database::manage::last_error() const
{
	return this->m_last_error;
}

/**
** 获取错误信息
** @return 错误信息（无需释放指针）
*/
const char *mp::database::manage::errmsg() const
{
	return sqlite3_errmsg(this->m_connect);
}

/**
** 获取最后的错误描述
** @return 错误描述（无需释放指针）
*/
const char *mp::database::manage::errstr() const
{
	return sqlite3_errstr(this->m_last_error);
}

/**
** 取指定错误代码的错误描述
** @param _rc 错误代码
** @return 错误描述（无需释放指针）
*/
const char *mp::database::manage::errstr(int _rc) const
{
	return sqlite3_errstr(_rc);
}

/**
** 获取当前管理的数据库连接指针
** @return sqlite3指针
*/
sqlite3 *mp::database::manage::connect_ptr() const
{
	return this->m_connect;
}

/**
** 设置当前管理的数据库连接指针
** @param _connect 将要更换的数据库指针
** @return 旧的数据库指针
*/
sqlite3 *mp::database::manage::connect_ptr(sqlite3 *_connect)
{
	sqlite3 *bak = this->m_connect;
	this->m_connect = _connect;
	return bak;
}

/**
** 开启事务处理
*/
bool mp::database::manage::begin_transaction()
{
	return this->exec_noquery("BEGIN TRANSACTION");
}

/**
** 结束/提交事务处理
*/
bool mp::database::manage::end_transaction()
{
	return this->exec_noquery("END TRANSACTION");
}

/**
** 回滚事务处理
*/
bool mp::database::manage::rollback_transaction()
{
	return this->exec_noquery("ROLLBACK");
}
