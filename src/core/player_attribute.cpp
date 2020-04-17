#include "player_attribute.h"
#include "../util/log.h"
#include <sstream>
#include <algorithm>

using std::string;
using std::stringstream;

/*
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在player_register表
** @param _properties 将要读取的玩家属性名称
** @param read_data 查询完毕后调用此函数，第一个参数是查询结果记录集已指向要读取的记录（无需释放），第二个参数是要读取的字段索引
** @return 是否查询成功
** @lock 此函数仅进行读操作，且不调用任何全局资源，无锁 */
bool mp::read_player_attribute(int64_t _player_id, string& _properties, std::function<void(database::stmt&, int)> call_back)
{
	//若程序非运行状态，取消操作
	if(!app_info.status_runtime) return false;
	//若程序当前状态为禁止读数据库，取消操作
	if(app_info.status_ban_read_database) return false;

	bool status_flag = false;

	int rc = 0;
	
	database::stmt stmt = connect_manage.prepare("SELECT attribute_value FROM db_player_data.player_attribute WHERE player_id=@player_id AND attribute_name=@attribute_name LIMIT 1");
	if(stmt.open_success()) 
	{
		stmt.bind_name({"@player_id", "@attribute_name"}, _player_id, _properties);
		//stmt.bind(1, 2);
		if(stmt.step() == SQLITE_ROW)
		{
			call_back(stmt, 0);
			status_flag = true;
		}
		else
		{
			/* 未查找到记录，应输出日志 */
			log(log::wran, "player_attribute", "read_player_attribute_int32") << "查询ID为" << _player_id << "的玩家的" << _properties << "属性时发现该玩家没有这个属性的记录" << log::push;
			status_flag = false;
		}
		
	}
	else
	{
		/* sqlite3_prepare_v2失败，应输出日志 */
		log(log::error, "player_attribute", "read_player_attribute_int32") << "查询ID为" << _player_id << "的玩家的" << _properties << "属性时发生错误：" << connect_manage.errstr() << log::push;
		status_flag = false;
	}

	return status_flag;
}

/*
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param bind_new_data 开始修改或新增数据时，会调用此函数，第一个参数是sqlite3_stmt，第二个参数是数据参数绑定索引
** @return 是否写入成功
** @lock 此函数将进行写入操作，但不调用全局资源，开启数据库写锁，函数执行期间不允许其它写行为 */
bool mp::write_player_attribute(int64_t _player_id, string& _properties, std::function<void(sqlite3_stmt*, int)> bind_new_data)
{
	//若程序非运行状态，取消操作
	if(!app_info.status_runtime) return false;
	//若程序当前状态为禁止写数据库，取消操作
	if(app_info.status_ban_write_database) return false;

	mp::lock_write.lock();
	bool status_flag = false;

	int rc = 0;

	sqlite3_stmt *quer_stmt = nullptr, *exec_stmt = nullptr;
	
	rc = sqlite3_prepare_v2(mp::connect, "SELECT attribute_value FROM db_player_data.player_attribute WHERE player_id=@player_id AND attribute_name=@attribute_name LIMIT 1", -1, &quer_stmt, nullptr);
	
	if(rc == SQLITE_OK) 
	{
		sqlite3_bind_int64(quer_stmt, sqlite3_bind_parameter_index(quer_stmt, "@player_id"), _player_id);
		sqlite3_bind_text(quer_stmt, sqlite3_bind_parameter_index(quer_stmt, "@attribute_name"), _properties.c_str(), _properties.size(), SQLITE_TRANSIENT);
		rc = sqlite3_step(quer_stmt);
		
		if(rc == SQLITE_ROW)
		{
			/* 查找到记录，执行修改操作 */
			rc = sqlite3_prepare_v2(mp::connect, "UPDATE db_player_data.player_attribute SET attribute_value = @attribute_value WHERE player_id = @player_id AND attribute_name = @attribute_name", -1, &exec_stmt, nullptr);
		}
		else
		{
			/* 未查找到记录，执行插入操作 */
			rc = sqlite3_prepare_v2(mp::connect, "INSERT INTO db_player_data.player_attribute(player_id, attribute_name, attribute_value) VALUES (@player_id, @attribute_name, @attribute_value)", -1, &exec_stmt, nullptr);
		}

		if (rc == SQLITE_OK)
		{
			sqlite3_bind_int64(exec_stmt, sqlite3_bind_parameter_index(exec_stmt, "@player_id"), _player_id);
			sqlite3_bind_text(exec_stmt, sqlite3_bind_parameter_index(exec_stmt, "@attribute_name"), _properties.c_str(), _properties.size(), SQLITE_TRANSIENT);
			bind_new_data(exec_stmt, sqlite3_bind_parameter_index(exec_stmt, "@attribute_value"));
			rc = sqlite3_step(exec_stmt);
			if(rc == SQLITE_DONE)
			{
				status_flag = true;
			}
			else
			{
				/* 执行插入或修改语句失败，应输出日志 */
				status_flag = false;
				log(log::error, "player_attribute", "read_player_attribute_text") << "写入ID为" << _player_id << "的玩家的" << _properties << "属性时在进行读写操作时发生错误：" << sqlite3_errmsg(mp::connect) << log::push;
			}
			
		}
		else
		{
			/* 准备插入或修改语句失败，应输出日志 */
			status_flag = false;
			log(log::error, "player_attribute", "read_player_attribute_text") << "写入ID为" << _player_id << "的玩家的" << _properties << "属性时在准备执行语句时发生错误：" << sqlite3_errmsg(mp::connect) << log::push;
		}
		
	}
	else
	{
		/* 准备查询语句失败，应输出日志 */
		status_flag = false;
		log(log::error, "player_attribute", "read_player_attribute_text") << "写入ID为" << _player_id << "的玩家的" << _properties << "属性时在查询该玩家指定属性是否存在时发生错误：" << sqlite3_errmsg(mp::connect) << log::push;
	}
	
	sqlite3_finalize(quer_stmt);
	sqlite3_finalize(exec_stmt);
	mp::lock_write.unlock();
	return status_flag;
}

/*
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
int32_t mp::read_player_attribute_int32(int64_t _player_id, string _properties, int32_t _default)
{
	int32_t result = _default;
	read_player_attribute(_player_id, _properties, [&result](database::stmt& _stmt, int _index){
		result = _stmt.column_int32(_index);
	});
	return result;
}

/*
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
int64_t mp::read_player_attribute_int64(int64_t _player_id, string _properties, int64_t _default)
{
	int64_t result = _default;
	read_player_attribute(_player_id, _properties, [&result](database::stmt& _stmt, int _index){
		result = result = _stmt.column_int64(_index);
	});
	return result;
}

/*
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
double mp::read_player_attribute_double(int64_t _player_id, string _properties, double _default)
{
	double result = _default;
	read_player_attribute(_player_id, _properties, [&result](database::stmt& _stmt, int _index){
		result = _stmt.column_double(_index);
	});

	return result;
}

/*
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
string mp::read_player_attribute_text(int64_t _player_id, string _properties, string _default)
{
	string result = _default;
	read_player_attribute(_player_id, _properties, [&result](database::stmt& _stmt, int _index){
		result = _stmt.column_text(_index);
	});

	return result;
}


/*
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return 是否写入成功 */
bool mp::write_player_attribute_int32(int64_t _player_id, std::string _properties, int32_t _data)
{
	return write_player_attribute(_player_id, _properties, [&_data](sqlite3_stmt* _stmt, int _index){
		sqlite3_bind_int(_stmt, _index, _data);
	});
}

/*
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return 是否写入成功 */
bool mp::write_player_attribute_int64(int64_t _player_id, std::string _properties, int64_t _data)
{
	return write_player_attribute(_player_id, _properties, [&_data](sqlite3_stmt* _stmt, int _index){
		sqlite3_bind_int64(_stmt, _index, _data);
	});
}

/*
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return 是否写入成功 */
bool mp::write_player_attribute_double(int64_t _player_id, std::string _properties, double _data)
{
	return write_player_attribute(_player_id, _properties, [&_data](sqlite3_stmt* _stmt, int _index){
		sqlite3_bind_double(_stmt, _index, _data);
	});
}

/*
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return 是否写入成功 */
bool mp::write_player_attribute_text(int64_t _player_id, std::string _properties, string _data)
{
	return write_player_attribute(_player_id, _properties, [&_data](sqlite3_stmt* _stmt, int _index){
		sqlite3_bind_text(_stmt, _index, _data.c_str(), _data.length(), SQLITE_TRANSIENT);
	});
}
