#include "basic.h"
#include <sstream>

std::mutex mpdb::lock;
sqlite3* mpdb::connect = nullptr;

using std::string;
using std::stringstream;

/*
 初始化数据库
 */
bool mpdb::init_database(string _game_db, string _user_db)
{

	if (_user_db.find('\'') != string::npos) return false;

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
 初始化数据库结构
 */
bool mpdb::init_database_struct()
{
	stringstream sql;
	//玩家登记表，为每个登记的玩家分配唯一的ID，以及绑定
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.user_register(id INTEGER PRIMARY KEY AUTOINCREMENT, bind_info VARCHAR);";
	
	//创建用户基础属性表
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.basic_attribute(user_id INTEGER,attribute_name VARCHAR,attribute_value VARCHAR);";
	
	char* error;
	int rc = sqlite3_exec(mpdb::connect, sql.str().c_str(), nullptr, nullptr, &error);
	if (rc != SQLITE_OK)
	{
		//此处应输出错误信息
	}

	sqlite3_free(error);

	return rc == SQLITE_OK;
}