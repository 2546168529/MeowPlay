#include "user_attribute.h"
#include <sstream>

using std::string;
using std::stringstream;

/*
** 读取用户基础属性
** @param _user 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 在查询失败后，默认返回的内容
** @return 查询结果 */
int32_t mpdb::read_user_attribute_int32(int64_t _user, string _properties, int32_t _default)
{
	mpdb::lock.lock();
	int32_t result = _default;

	int rc = 0;

	sqlite3_stmt *stmt = nullptr;
	
	rc = sqlite3_prepare_v2(mpdb::connect, "SELECT attribute_value FROM db_player_data.basic_attribute WHERE user_id=@user_id AND attribute_name=@attribute_name LIMIT 1", -1, &stmt, nullptr);
	
	if(rc == SQLITE_OK) 
	{
		sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@user_id"), _user);
		sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@attribute_name"), _properties.c_str(), _properties.size(), SQLITE_TRANSIENT);
		if(sqlite3_step(stmt) == SQLITE_ROW)
		{
			result = sqlite3_column_int(stmt, 0);
		}
		else
		{
			/* 未查找到记录，应输出日志 */
		}
		
	}
	else
	{
		/* sqlite3_prepare_v2失败，应输出日志 */
	}
	
	sqlite3_finalize(stmt);
	mpdb::lock.unlock();
	return result;
}

/*
** 读取用户基础属性
** @param _user 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 在查询失败后，默认返回的内容
** @return 查询结果 */
int64_t mpdb::read_user_attribute_int64(int64_t _user, string _properties, int64_t _default)
{
	mpdb::lock.lock();
	int64_t result = _default;

	int rc = 0;

	sqlite3_stmt *stmt = nullptr;
	
	rc = sqlite3_prepare_v2(mpdb::connect, "SELECT attribute_value FROM db_player_data.basic_attribute WHERE user_id=@user_id AND attribute_name=@attribute_name LIMIT 1", -1, &stmt, nullptr);
	
	if(rc == SQLITE_OK) 
	{
		sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@user_id"), _user);
		sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@attribute_name"), _properties.c_str(), _properties.size(), SQLITE_TRANSIENT);
		if(sqlite3_step(stmt) == SQLITE_ROW)
		{
			result = sqlite3_column_int64(stmt, 0);
		}
		else
		{
			/* 未查找到记录，应输出日志 */
		}
		
	}
	else
	{
		/* sqlite3_prepare_v2失败，应输出日志 */
	}
	
	sqlite3_finalize(stmt);
	mpdb::lock.unlock();
	return result;
}

/*
** 读取用户基础属性
** @param _user 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 在查询失败后，默认返回的内容
** @return 查询结果 */
string mpdb::read_user_attribute_text(int64_t _user, string _properties, const char* _default)
{
	mpdb::lock.lock();
	string result(_default);

	int rc = 0;

	sqlite3_stmt *stmt = nullptr;
	
	rc = sqlite3_prepare_v2(mpdb::connect, "SELECT attribute_value FROM db_player_data.basic_attribute WHERE user_id=@user_id AND attribute_name=@attribute_name LIMIT 1", -1, &stmt, nullptr);
	
	if(rc == SQLITE_OK) 
	{
		sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@user_id"), _user);
		sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@attribute_name"), _properties.c_str(), _properties.size(), SQLITE_TRANSIENT);
		if(sqlite3_step(stmt) == SQLITE_ROW)
		{
			result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		}
		else
		{
			/* 未查找到记录，应输出日志 */
		}
		
	}
	else
	{
		/* sqlite3_prepare_v2失败，应输出日志 */
	}
	
	sqlite3_finalize(stmt);
	mpdb::lock.unlock();

	//return std::move(result);
	return result;
}
