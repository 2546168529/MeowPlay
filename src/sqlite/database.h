#pragma one
#include "sqlite3.h"
#include <inttypes.h>
#include <string>
#include <initializer_list>
#include <mutex>
#include <atomic>
#include <memory>

namespace mp{
    namespace database{

        class stmt 
        {
            sqlite3_stmt* m_stmt;
            int m_last_error;
            bool m_open_success;
			size_t* m_use;
            
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

			void free();

        public:
            stmt(sqlite3* _connect, const char* sql);
			stmt(const stmt& _stmt) noexcept;
			stmt(stmt&& _stmt) noexcept;

			stmt& operator=(const stmt& _stmt) noexcept;
			stmt& operator=(stmt&& _stmt) noexcept;

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
			bool bind(const std::initializer_list<const char*> _param_name, const __args&... _args)
			{
				if (this->m_stmt == nullptr) return false;
				return this->bind_recur_name(0, _param_name, std::forward<decltype(_args)>(_args)...);
			}

            bool open_success()
            {
                return this->m_open_success;
            }
            
			void column(int _iCol, int32_t& _nRec)
			{
				_nRec = sqlite3_column_int(this->m_stmt, _iCol);
			}

			void column(int _iCol, int64_t& _nRec)
			{
				_nRec = sqlite3_column_int64(this->m_stmt, _iCol);
			}

			void column(int _iCol, double& _nRec)
			{
				_nRec = sqlite3_column_double(this->m_stmt, _iCol);
			}

			void column(int _iCol, std::string& _nRec)
			{
				_nRec = reinterpret_cast<const char*>(sqlite3_column_text(this->m_stmt, _iCol));
			}

            template<class __T>
			__T column(int _iCol)
			{
				__T nRec;
				column(_iCol, nRec);
				return nRec;
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

			template<class... __args>
			bool exec_noquery(const char* _sql, const __args &... _args)
			{
				stmt exec_stmt = prepare(_sql);
				exec_stmt.bind(std::forward<decltype(_args)>(_args)...);
				return exec_stmt.step() == SQLITE_DONE;
			}

			template<class... __args>
			bool exec_noquery(const char* _sql, std::initializer_list<const char*> __ascii_iswdigit, const __args &... _args)
			{
				stmt exec_stmt = prepare(_sql);
				exec_stmt.bind(__ascii_iswdigit, std::forward<decltype(_args)>(_args)...);
				return exec_stmt.step() == SQLITE_DONE;
			}

            stmt prepare(const char* _sql);

			template<class... __args>
			stmt prepare(const char* _sql, const __args &... _args)
			{
				stmt exec_stmt = prepare(_sql);
				if(exec_stmt.open_success())
				{
					exec_stmt.bind(std::forward<decltype(_args)>(_args)...);
				}
				return exec_stmt;
			}

            bool close();

            int last_error();

            const char* errmsg();

            const char* errstr();

            const char* errstr(int _rc);

            sqlite3* connect_ptr();

            sqlite3* connect_ptr(sqlite3* _connect);
        };
        
    }
}


