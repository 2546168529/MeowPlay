#include "database.h"

mp::database::stmt::stmt(sqlite3* _connect, const char* sql)
{
    const char *tail;
    int rc = sqlite3_prepare_v2(_connect, sql, -1, &this->m_stmt, &tail);
    this->m_last_error = rc;
    m_open_success = (rc == SQLITE_OK);
}

mp::database::stmt::~stmt()
{
    this->finalize();
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

