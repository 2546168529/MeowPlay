#pragma once

#include <thread>
#include <mutex>

#include <inttypes.h>
#include <string>
#include <atomic>

#include "../util/log.h"
#include "../sqlite/sqlite3.h"
#include "../sqlite/database.h"

namespace mp {
	typedef uint32_t Status;
	constexpr uint32_t status_ok = 0;
	constexpr uint32_t status_error = 1;
	constexpr uint32_t status_nonexistent = 2;
	constexpr uint32_t status_exist = 3;
	constexpr uint32_t status_ban = 4;
	constexpr uint32_t status_query_error = 5;
	constexpr uint32_t status_exec_error = 6;
	constexpr uint32_t status_begin_transaction_error = 7;

	/*
	** 程序信息结构
	** 由于此结构将用于全局资源，所以成员必须需要原子操作 */
	struct app_info_struct
	{
		//程序是否正在运行
		std::atomic_bool status_runtime = false;
	};
	extern app_info_struct app_info;
	
	extern const std::string nullstr;

	extern std::recursive_mutex lock_write;
	extern mp::database::manage db_manage;

	bool init_database(std::string _game_db, std::string _user_db);
	bool init_database_struct();

}

