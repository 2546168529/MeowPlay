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
** 保证玩家数据库(db_play_data)已附加 */
mp::database::manage mp::db_manage;

const std::string mp::nullstr("");

using std::string;
using std::stringstream;

/**
** 初始化数据库
** @param _base_db 游戏基础数据库路径，用于保存游戏数据
** @param _player_db 玩家数据库路径，仅用于存储玩家数据
** @return 若为true，保证游戏数据库已打开，且玩家数据库(db_play_data)已附加 */
bool mp::init_database(string _base_db, string _player_db)
{

	if (_base_db.find('\'') != string::npos && _player_db.find('\'') != string::npos)
	{
		/* 数据库路径里不能携带'符号，此处应该输出日志 */
		log(log::error, "core-basic", "init_database") << "数据库路径里不能携带'符号" << log::push;
		return false;
	}
	
	if (db_manage.open(":memory:", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE))
	{
		stringstream ss;
		ss << "ATTACH DATABASE '" << _player_db << "' AS db_play_data;";
		ss << "ATTACH DATABASE '" << _base_db << "' AS db_base_data;";
		if (db_manage.exec_noquery(ss.str().data()))
		{
			return init_database_struct();
		}

		db_manage.close();
	}

	return false;
}

/**
** 初始化数据库结构
** @return 若为true，保证游戏数据库与玩家数据库结构完整 */
bool mp::init_database_struct()
{
	lock_write.lock();
	stringstream sql;
	
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.player_register(player_id INTEGER PRIMARY KEY AUTOINCREMENT,bind_info VARCHAR);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.player_attribute(player_id INTEGER, attribute_name VARCHAR,attribute_value VARCHAR);";
	
	/* 物品系统 */
	sql << "CREATE TABLE IF NOT EXISTS db_base_data.items_info (item_base_id INTEGER PRIMARY KEY AUTOINCREMENT,item_name VARCHAR,item_type INTEGER,uniqueness INTEGER,base_info VARCHAR);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.items_uuid_base (item_base_id INTEGER,uuid_base INTEGER);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.player_items (item_uuid INTEGER,player_id INTEGER,amount INTEGER,position INTEGER,position_weight INTEGER,other INTEGER);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.items_register (item_uuid INTEGER,item_base_id INTEGER,regist_time DATETIME,time_limit INTEGER,weight INTEGER);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.items_attributes (item_uuid INTEGER,attributes_name VARCHAR,attributes_value VARCHAR);";
	
	bool exec_status = db_manage.exec_noquery(sql.str().data());
	if (!exec_status)
	{
		log(log::error, "core-basic", "init_database") << "初始化数据库结构失败:" << db_manage.errstr() << log::push;
		app_info.status_runtime = false;
		db_manage.close();
	}
	else
	{
		app_info.status_runtime = true;
	}

	lock_write.unlock();
	
	return exec_status;
}
