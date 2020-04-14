#include "basic.h"
#include <sstream>

/* 
** 用于数据库读写的锁
** 主要用途：
** 绝对避免在事务处理时进行其它读写操作
** 绝对避免同时发生写入操作 */
std::mutex mp::lock_db;

/*
** SQLITE数据库连接句柄
** 保证玩家数据库(db_player_data)已附加 */
sqlite3* mp::connect = nullptr;

using std::string;
using std::stringstream;

/*
** 初始化数据库
** @param _game_db 游戏数据库路径，用于保存游戏数据
** @param _user_db 玩家数据库路径，仅用于存储玩家数据
** @return 若为true，保证游戏数据库已打开，且玩家数据库(db_player_data)已附加 */
bool mp::init_database(string _game_db, string _user_db)
{

	if (_user_db.find('\'') != string::npos)
	{
		/* 数据库路径里不能携带'符号，此处应该输出日志 */
		log(log::error, "core-basic", "init_database") << "数据库路径里不能携带'符号" << log::push;
		return false;
	}

	int rc;
	rc = sqlite3_open_v2(_game_db.c_str(), &connect, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
	if (rc == SQLITE_OK)
	{
		sqlite3_stmt* stmt;
		stringstream ss;
		ss << "ATTACH DATABASE '" << _user_db << "' AS db_player_data";
		
		rc = sqlite3_prepare_v2(connect, ss.str().c_str(), -1, &stmt, 0);
		if (rc == SQLITE_OK)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			if (rc == SQLITE_DONE)
			{
				return true;
			}
		}
		sqlite3_finalize(stmt);
		sqlite3_close(connect);
	}

	return false;
}

/*
** 初始化数据库结构
** @return 若为true，保证游戏数据库与玩家数据库结构完整 */
bool mp::init_database_struct()
{
	stringstream sql;
	//玩家登记表，为每个登记的玩家分配唯一的ID，以及绑定
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.user_register(id INTEGER PRIMARY KEY AUTOINCREMENT, bind_info VARCHAR);";
	
	//创建用户基础属性表
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.basic_attribute(user_id INTEGER,attribute_name VARCHAR,attribute_value VARCHAR);";
	
	char* error;
	int rc = sqlite3_exec(mp::connect, sql.str().c_str(), nullptr, nullptr, &error);
	if (rc != SQLITE_OK)
	{
		//此处应输出错误信息
		log(log::error, "core-basic", "init_database") << "初始化数据库结构失败:" << sqlite3_errmsg(mp::connect) << log::push;
	}

	sqlite3_free(error);

	return rc == SQLITE_OK;
}