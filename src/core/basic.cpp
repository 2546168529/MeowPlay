#include "basic.h"
#include <sstream>

/*
** 程序运行状态 */
mp::app_info_struct mp::app_info;

/* 
** 数据库写锁
** 主要用途：
** 绝对避免在事务处理时进行其他写操作
** 绝对避免同时发生写入操作 */
std::mutex mp::lock_write;

/*
** SQLITE数据库连接句柄
** 作为特殊全局资源，不允许在运行时改变其值
** 保证玩家数据库(db_player_data)已附加 */
sqlite3* mp::connect = nullptr;
mp::database::manage mp::connect_manage;

const std::string mp::nullstr("");

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
	connect_manage.connect_ptr(connect);

	if (rc == SQLITE_OK)
	{
		stringstream ss;
		ss << "ATTACH DATABASE '" << _user_db << "' AS db_player_data";
		
		if (connect_manage.exec_noquery(ss.str().data()))
		{
			return init_database_struct();
		}

		connect_manage.close();
		connect = nullptr;
	}

	return false;
}

/*
** 初始化数据库结构
** @return 若为true，保证游戏数据库与玩家数据库结构完整 */
bool mp::init_database_struct()
{
	lock_write.lock();
	stringstream sql;
	//玩家登记表，为每个登记的玩家分配唯一的ID，以及绑定
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.player_register(player_id INTEGER PRIMARY KEY AUTOINCREMENT, bind_info VARCHAR);";
	
	//创建用户基础属性表
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.player_attribute(player_id INTEGER,attribute_name VARCHAR,attribute_value VARCHAR);";
	
	//创建玩家背包表
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.player_items (player_item_id INTEGER PRIMARY KEY AUTOINCREMENT,player_id INTEGER,item_id INTEGER,amount INTEGER,position_weight INTEGER);";

	bool exec_status = connect_manage.exec_noquery(sql.str().data());
	if (!exec_status)
	{
		log(log::error, "core-basic", "init_database") << "初始化数据库结构失败:" << sqlite3_errmsg(mp::connect) << log::push;
		app_info.status_runtime = false;
		connect_manage.close();
		connect = nullptr;
	}
	else
	{
		app_info.status_runtime = true;
	}

	lock_write.unlock();
	
	return exec_status;
}
