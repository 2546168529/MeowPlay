#include "database.h"

mp::database::stmt::stmt(sqlite3* _connect, const char* sql)
    : m_use(new size_t(1))
{
    const char *tail;
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
    ++*_stmt.m_use;//先将传入的stmt的所用记次+1，避免自赋值造成的异常

    if(--*(this->m_use) == 0)
    {
        delete this->m_use;
        this->finalize();
    }

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
	if(this != &_stmt)
	{
		this->free();

		this->m_use = _stmt.m_use;
    	this->m_open_success = _stmt.m_open_success;
    	this->m_last_error = _stmt.m_last_error;
    	this->m_stmt = _stmt.m_stmt;
	}
	return *this;
}

mp::database::stmt::stmt(stmt&& _stmt) noexcept
	: m_stmt(_stmt.m_stmt), m_use(_stmt.m_use), m_last_error(_stmt.m_last_error), m_open_success(_stmt.m_open_success)
{
	_stmt.m_use = nullptr;
	_stmt.m_stmt = nullptr;
}

void mp::database::stmt::free()
{
    if(this->m_use != nullptr && --*(this->m_use) == 0)
    {
        delete this->m_use;
        this->finalize();
        this->m_use = nullptr;
    }
}

bool mp::database::stmt::finalize()
{
    int rc = sqlite3_finalize(this->m_stmt);
    if(rc != SQLITE_OK)
    {
        this->m_last_error = rc;
        return true;
    }
    else
    {
        this->m_stmt = nullptr;
        return false;
    }
}

int mp::database::stmt::step()
{
    return sqlite3_step(this->m_stmt);
}

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

bool mp::database::manage::exec_noquery(const char* _sql)
{
    char *pchar_err_message = nullptr;
    int rc = sqlite3_exec(this->m_connect,
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

mp::database::stmt mp::database::manage::prepare(const char* _sql)
{
    return stmt(this->m_connect, _sql);
}

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


int mp::database::manage::last_error()
{
    return this->m_last_error;
}

const char* mp::database::manage::errmsg()
{
    return sqlite3_errmsg(this->m_connect);
}

const char* mp::database::manage::errstr()
{
    return sqlite3_errstr(this->m_last_error);
}

const char* mp::database::manage::errstr(int _rc)
{
    return sqlite3_errstr(_rc);
}

sqlite3* mp::database::manage::connect_ptr()
{
    return this->m_connect;
}

sqlite3* mp::database::manage::connect_ptr(sqlite3* _connect)
{
    this->m_connect = _connect;
    return _connect;
}

