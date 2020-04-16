#pragma one
#include "sqlite3.h"
#include <inttypes.h>
#include <string>

namespace mp{
    namespace database{

        class manage
        {
        private:
            sqlite3* m_connect;
            
        public:
            manage(sqlite3* _connect) : m_connect(_connect) {};
            ~manage();

            int exec_noquery(const char* _sql);

            class stmt prepare(const char* _sql);
        };

        class stmt 
        {
            sqlite3_stmt* m_stmt;

        private:
            template<class... __args>
            void bind(int _which, int32_t _value, const __args &... _args)
            {
                sqlite3_bind_int(this->m_stmt, _which, _value);
                this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            void bind(int _which, int64_t& _value, const __args &... _args)
            {
                sqlite3_bind_int64(this->m_stmt, _which, _value);
                this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            void bind(int _which, double& _value, const __args &... _args)
            {
                sqlite3_bind_double(this->m_stmt, _which, _value);
                this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            void bind(int _which, std::string& _value, const __args &... _args)
            {
                sqlite3_bind_text(this->m_stmt, _which, _value.data(), -1, SQLITE_STATIC);
                this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            void bind(int _which, const char* & _value, const __args &... _args)
            {
                sqlite3_bind_text(this->m_stmt, _which, _value, -1, SQLITE_STATIC);
                this->bind(_which + 1, std::forward<decltype(_args)>(_args)...);
            }

            template<class... __args>
            void bind(int _which){}

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
                int temp = sqlite3_finalize(this->m_stmt);
                this->m_stmt = nullptr;
                return temp;
            } 

            template<class... __args>
            stmt bind(const __args &... _args)
            {
                this->bind(1, _args...);
            }
            
        };
        
    }
}


