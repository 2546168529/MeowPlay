#include "player_attribute.h"
#include "../util/log.h"

using std::string;

/**
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在player_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 如果查询失败，将返回的默认值
** @return 查询结果
** @lock 此函数仅进行读操作，且不调用任何全局资源，无锁 */
template <class __pro, class __dt>
inline __dt read_player_attribute(int64_t _player_id, const __pro _properties, const __dt _default)
{
	__dt result = _default;

	/* 若程序非运行状态，取消操作 */
	if (!mp::app_info.status_runtime)
		return result;

	mp::database::stmt stmt = mp::db_manage.prepare(
		"SELECT attribute_value FROM db_play_data.player_attribute WHERE player_id=@player_id AND attribute_name=@attribute_name LIMIT 1",
		{"@player_id", "@attribute_name"},
		_player_id, _properties);
	if (stmt.open_success())
	{
		int rc = stmt.step();
		if (rc == SQLITE_ROW)
		{
			stmt.column(0, result);
		}
		else if (rc != SQLITE_DONE)
		{
			/* 执行发生错误 */
			mp::log(mp::log::error, "player_attribute", "read_player_attribute_int32") << "查询ID为" << _player_id << "的玩家的" << _properties << "属性时在执行步骤发生错误，rc=" << rc << mp::log::push;
		}
	}
	else
	{
		/* sqlite3_prepare_v2失败，应输出日志 */
		mp::log(mp::log::error, "player_attribute", "read_player_attribute_int32") << "查询ID为" << _player_id << "的玩家的" << _properties << "属性时发生错误：" << mp::db_manage.errstr() << mp::log::push;
	}

	return result;
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param bind_new_data 开始修改或新增数据时，会调用此函数，第一个参数是sqlite3_stmt，第二个参数是数据参数绑定索引
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码
** @lock 此函数将进行写入操作，但不调用全局资源，开启数据库写锁，函数执行期间不允许其它写行为 */
template <class __pro, class __dt>
inline mp::Status write_player_attribute(int64_t _player_id, const __pro _properties, const __dt _data)
{
	/* 若程序非运行状态，取消操作 */
	if (!mp::app_info.status_runtime)
		return mp::status_ban;
	/* 自动加锁 */
	std::lock_guard<std::recursive_mutex> lock(mp::lock_write);

	mp::database::stmt quer_stmt = mp::db_manage.prepare(
		"SELECT attribute_value FROM db_play_data.player_attribute WHERE player_id=@player_id AND attribute_name=@attribute_name LIMIT 1",
		{"@player_id", "@attribute_name"},
		_player_id, _properties);

	if (quer_stmt.open_success())
	{

		const char *exec_sql;
		int rc = quer_stmt.step();
		if (rc == SQLITE_ROW)
		{
			/* 查找到记录，执行修改操作 */
			exec_sql = "UPDATE db_play_data.player_attribute SET attribute_value = @attribute_value WHERE player_id = @player_id AND attribute_name = @attribute_name";
		}
		else if (rc == SQLITE_DONE)
		{
			/* 未查找到记录，执行插入操作 */
			exec_sql = "INSERT INTO db_play_data.player_attribute(player_id, attribute_name, attribute_value) VALUES (@player_id, @attribute_name, @attribute_value)";
		}
		else
		{
			/* 查询操作执行失败 */
			mp::log(mp::log::error, "player_attribute", "read_player_attribute_text") << "写入ID为" << _player_id << "的玩家的" << _properties << "属性时在执行查询步骤发生错误，rc=" << rc << mp::log::push;
			return mp::status_query_error;
		}

		if (!mp::db_manage.exec_noquery(exec_sql, {"@player_id", "@attribute_name", "@attribute_value"}, _player_id, _properties, _data))
		{
			/* 执行操作执行失败 */
			mp::log(mp::log::error, "player_attribute", "read_player_attribute_text") << "写入ID为" << _player_id << "的玩家的" << _properties << "属性时在进行读写操作时发生错误：" << mp::db_manage.errstr() << mp::log::push;
			return mp::status_exec_error;
		}
	}
	else
	{
		/* 准备查询语句失败，应输出日志 */
		mp::log(mp::log::error, "player_attribute", "read_player_attribute_text") << "写入ID为" << _player_id << "的玩家的" << _properties << "属性时在查询该玩家指定属性是否存在时发生错误：" << mp::db_manage.errstr() << mp::log::push;
		return mp::status_query_error;
	}

	return mp::status_ok;
}

/**
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
int32_t mp::read_player_attribute_int32(int64_t _player_id, string _properties, int32_t _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
int64_t mp::read_player_attribute_int64(int64_t _player_id, string _properties, int64_t _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
double mp::read_player_attribute_double(int64_t _player_id, string _properties, double _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
string mp::read_player_attribute_text(int64_t _player_id, string _properties, string _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
int32_t mp::read_player_attribute_int32(int64_t _player_id, const char *_properties, int32_t _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
int64_t mp::read_player_attribute_int64(int64_t _player_id, const char *_properties, int64_t _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
double mp::read_player_attribute_double(int64_t _player_id, const char *_properties, double _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** 读取用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要读取的玩家属性名称
** @param _default 查询失败后默认返回的内容
** @return 查询结果 */
string mp::read_player_attribute_text(int64_t _player_id, const char *_properties, string _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_int32(int64_t _player_id, std::string _properties, int32_t _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_int64(int64_t _player_id, std::string _properties, int64_t _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_double(int64_t _player_id, std::string _properties, double _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_text(int64_t _player_id, std::string _properties, string _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_text(int64_t _player_id, std::string _properties, const char *_data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_int32(int64_t _player_id, const char *_properties, int32_t _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_int64(int64_t _player_id, const char *_properties, int64_t _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_double(int64_t _player_id, const char *_properties, double _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_text(int64_t _player_id, const char *_properties, string _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** 写用户基础属性
** @param _player_id 用户ID，此ID为玩家在注册时系统自动分配的id，记录在user_register表
** @param _properties 将要修改的的玩家属性名称
** @param _data 要写入的数据
** @return mp::status_*，成功返回mp::status_ok，失败返回错误代码 */
mp::Status mp::write_player_attribute_text(int64_t _player_id, const char *_properties, const char *_data)
{
	return write_player_attribute(_player_id, _properties, _data);
}
