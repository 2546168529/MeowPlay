#pragma one
#include "sqlite3.h"
#include <inttypes.h>
#include <string>
#include <initializer_list>
#include <mutex>
#include <atomic>

namespace mp{
    namespace database{

        class stmt 
        {
            sqlite3_stmt* m_stmt;
            std::atomic_int m_last_error;
            bool m_open_success;
            
            /* 根据索引的参数绑定 */
            template<class... __args>
			bool bind_recur(int _which, const int32_t& _value, const __args&... _args)
			{
				int rc = sqlite3_bind_int(this->m_stmt, _which, _value);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur(_which + 1, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
			bool bind_recur(int _which, const int64_t& _value, const __args&... _args)
			{
				int rc = sqlite3_bind_int64(this->m_stmt, _which, _value);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur(_which + 1, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
			bool bind_recur(int _which, const double& _value, const __args&... _args)
			{
				int rc = sqlite3_bind_double(this->m_stmt, _which, _value);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur(_which + 1, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
			bool bind_recur(int _which, const std::string& _value, const __args&... _args)
			{
				int rc = sqlite3_bind_text(this->m_stmt, _which, _value.data(), -1, SQLITE_TRANSIENT);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur(_which + 1, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
			bool bind_recur(int _which, const char*& _value, const __args&... _args)
			{
				int rc = sqlite3_bind_text(this->m_stmt, _which, _value, -1, SQLITE_TRANSIENT);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur(_which + 1, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
            bool bind_recur(int _which){ return true; }

            /* 根据名称的参数绑定 */
            template<class... __args>
			bool bind_recur_name(size_t _which, const std::initializer_list<const char*>& _param_name, const int32_t& _value, const __args&... _args)
			{
				if (_param_name.size() == _which) return true;
				int rc = sqlite3_bind_int(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin() + _which)), _value);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur_name(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
			bool bind_recur_name(size_t _which, const std::initializer_list<const char*>& _param_name, const int64_t& _value, const __args&... _args)
			{
				if (_param_name.size() == _which) return true;
				int rc = sqlite3_bind_int64(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin() + _which)), _value);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur_name(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
			bool bind_recur_name(size_t _which, const std::initializer_list<const char*>& _param_name, const double& _value, const __args&... _args)
			{
				if (_param_name.size() == _which) return true;
				int rc = sqlite3_bind_double(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin() + _which)), _value);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur_name(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
			bool bind_recur_name(size_t _which, const std::initializer_list<const char*>& _param_name, const std::string& _value, const __args&... _args)
			{
				if (_param_name.size() == _which) return true;
				int rc = sqlite3_bind_text(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin() + _which)), _value.data(), -1, SQLITE_TRANSIENT);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur_name(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
			bool bind_recur_name(size_t _which, const std::initializer_list<const char*>& _param_name, const char*& _value, const __args&... _args)
			{
				if (_param_name.size() == _which) return true;
				int rc = sqlite3_bind_text(this->m_stmt, sqlite3_bind_parameter_index(this->m_stmt, *(_param_name.begin() + _which)), _value, -1, SQLITE_TRANSIENT);
				if (rc != SQLITE_OK)
				{
					this->m_last_error = rc;
					return false;
				}
				return this->bind_recur_name(_which + 1, _param_name, std::forward<decltype(_args)>(_args)...);
			}

            template<class... __args>
            bool bind_recur_name(size_t _which, const std::initializer_list<const char*>& _param_name){ return true; }

        public:
            stmt(sqlite3* _connect, const char* sql);

            ~stmt();

            bool finalize();

            int step();

            bool reset();

            template<class... __args>
            bool bind(const __args &... _args)
            {
                if(this->m_stmt == nullptr) return false;
                return this->bind_recur(1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
			bool bind_name(const std::initializer_list<const char*> _param_name, const __args&... _args)
			{
				if (this->m_stmt == nullptr) return false;
				return this->bind_recur_name(0, _param_name, std::forward<decltype(_args)>(_args)...);
			}

            bool open_success()
            {
                return this->m_open_success;
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

            sqlite3_stmt* stmt_ptr()
            {
                return this->m_stmt;
            }

            int last_error()
            {
                return this->m_last_error;
            }

        };

        class manage
        {
        private:
            sqlite3* m_connect;
            std::atomic_int m_last_error;

        public:
            manage() : m_connect(nullptr) {};
            manage(sqlite3* _connect) : m_connect(_connect) {};
            ~manage(){ close(); }

            bool exec_noquery(const char* _sql);

            stmt prepare(const char* _sql);

            bool close();

            int last_error();

            const char* errmsg();

            const char* errstr();

            const char* errstr(int rc);

            sqlite3* connect_ptr();

            sqlite3* connect_ptr(sqlite3* _connect);
        };
        
    }
}


