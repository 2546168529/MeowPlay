#pragma once

#include <mutex>
#include <inttypes.h>
#include <string>
#include <atomic>

#include "../util/log.h"
#include "../sqlite/sqlite3.h"
#include "../sqlite/database.h"

namespace mp {
	
	/*
	** 程序信息结构
	** 由于此结构将用于全局资源，所以成员必须需要原子操作 */
	struct app_info_struct
	{
		//程序是否正在运行
		std::atomic_bool status_runtime = false;
		//是否禁止读数据库
		std::atomic_bool status_ban_read_database = false;
		//是否禁止写数据库
		std::atomic_bool status_ban_write_database = false;
	};
	extern app_info_struct app_info;
	
	extern const std::string nullstr;

	extern std::mutex lock_write;
	extern sqlite3* connect;
	extern mp::database::manage connect_manage;

	bool init_database(std::string _game_db, std::string _user_db);
	bool init_database_struct();

}

