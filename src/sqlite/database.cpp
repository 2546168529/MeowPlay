#include "database.h"
        
mp::database::manage::~manage()
{
    
}

int mp::database::manage::exec_noquery(const char* _sql)
{
    char *pchar_err_message = nullptr;
    return sqlite3_exec(this->m_connect,
                        _sql,
                        [](void *data, int argc, char **argv, char **azColName) -> int { return SQLITE_OK; },
                        nullptr,
                        &pchar_err_message);
}

mp::database::stmt mp::database::manage::prepare(const char* _sql)
{
    return mp::database::stmt(this->m_connect, _sql);
}
