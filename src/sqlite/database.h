#pragma one
#include "sqlite3.h"
#include <inttypes.h>
#include <string>
#include <initializer_list>
#include <mutex>

namespace mp{
    namespace database{

        class stmt 
        {
            sqlite3_stmt* m_stmt;

        private:

            /* 根据索引的参数绑定 */
            template<class... __args>
            bool bind(int _which, int32_t& _value, const __args &... _args)
            {
                if(sqlite3_bind_int(this->m_stmt, _which, _value) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(int _which, int64_t& _value, const __args &... _args)
            {
                if(sqlite3_bind_int64(this->m_stmt, _which, _value) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(int _which, double& _value, const __args &... _args)
            {
                if(sqlite3_bind_double(this->m_stmt, _which, _value) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(int _which, std::string& _value, const __args &... _args)
            {
                if(sqlite3_bind_text(this->m_stmt, _which, _value.data(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(int _which, const char* & _value, const __args &... _args)
            {
                if(sqlite3_bind_text(this->m_stmt, _which, _value, -1, SQLITE_TRANSIENT) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(int _which){ return true; }

            /* 根据名称的参数绑定 */
            template<class... __args>
            bool bind(size_t _which, std::initializer_list<const char*>& _param_name, int32_t& _value, const __args &... _args)
            {
                if(_param_name.size() == _which) return false;
                if(sqlite3_bind_int(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin()+_which)), _value) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(size_t _which, std::initializer_list<const char*>& _param_name, int64_t& _value, const __args &... _args)
            {
                if(_param_name.size() == _which) return false;
                if(sqlite3_bind_int64(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin()+_which)), _value) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(size_t _which, std::initializer_list<const char*>& _param_name, double& _value, const __args &... _args)
            {
                if(_param_name.size() == _which) return false;
                if(sqlite3_bind_double(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin()+_which)), _value) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(size_t _which, std::initializer_list<const char*>& _param_name, std::string& _value, const __args &... _args)
            {
                if(_param_name.size() == _which) return false;
                if(sqlite3_bind_text(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin()+_which)), _value.data(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(size_t _which, std::initializer_list<const char*>& _param_name, const char* & _value, const __args &... _args)
            {
                if(_param_name.size() == _which) return false;
                if(sqlite3_bind_text(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin()+_which)), _value, -1, SQLITE_TRANSIENT) != SQLITE_OK)
                {
                    return false;
                }
                return this->bind(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            bool bind(size_t _which, std::initializer_list<const char*>& _param_name){ return true; }

        public:
            stmt(sqlite3* _connect, const char* sql)
            {
                const char *tail;
                sqlite3_prepare_v2(_connect, sql, -1, &this->m_stmt, &tail);
            }

            ~stmt()
            {
                this->finalize();
            }

            int finalize()
            {
                int rc = sqlite3_finalize(this->m_stmt);
                this->m_stmt = nullptr;
                return rc;
            } 

            template<class... __args>
            bool bind(const __args &... _args)
            {
                return this->bind(1, _args...);
            }

            template<class... __args>
            bool bind(std::initializer_list<const char*> _param_name, const __args &... _args)
            {
                return this->bind(0, _args...);
            }

            int step()
            {
                return sqlite3_step(this->m_stmt);
            }

            int reset()
            {
                return sqlite3_reset(this->m_stmt);
            }
            
            int32_t column_int32(int _iCol)
            {
                if(this->m_stmt == nullptr) return 0;
                return sqlite3_column_int(this->m_stmt, _iCol);
            }

            int64_t column_int64(int _iCol)
            {
                if(this->m_stmt == nullptr) return 0;
                return sqlite3_column_int64(this->m_stmt, _iCol);
            }

            double column_double(int _iCol)
            {
                if(this->m_stmt == nullptr) return 0;
                return sqlite3_column_double(this->m_stmt, _iCol);
            }

            const char* column_text(int _iCol)
            {
                if(this->m_stmt == nullptr) return nullptr;
                return reinterpret_cast<const char*>(sqlite3_column_text(this->m_stmt, _iCol));
            }

        };

        class manage
        {
        private:
            sqlite3* m_connect;
            
        public:
            manage(sqlite3* _connect) : m_connect(_connect) {};
            ~manage()
            {
                close();
            }

            int exec_noquery(const char* _sql)
            {
                char *pchar_err_message = nullptr;
                return sqlite3_exec(this->m_connect,
                        _sql,
                        [](void *data, int argc, char **argv, char **azColName) -> int { return SQLITE_OK; },
                        nullptr,
                        &pchar_err_message);
            }

            stmt prepare(const char* _sql)
            {
                return stmt(this->m_connect, _sql);
            }

            int close()
            {
                sqlite3_close_v2(this->m_connect);
                this->m_connect = nullptr;
            }
        };
        
    }
}


