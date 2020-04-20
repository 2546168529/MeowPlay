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

	//通用查询结果
	enum query_result{
		success,	//成功
		nonexistent,//不存在
		error,		//出错
		ban			//禁止操作
	};
	
	extern const std::string nullstr;

	extern std::recursive_mutex lock_write;
	extern mp::database::manage db_manage;

	bool init_database(std::string _game_db, std::string _user_db);
	bool init_database_struct();

}

