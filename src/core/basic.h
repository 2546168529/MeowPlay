#pragma once

#include <mutex>
#include <inttypes.h>
#include <string>
#include <atomic>

#include "../util/log.h"
#include "../sqlite/sqlite3.h"

namespace mp {
	extern std::atomic_uint32_t app_status;
	constexpr uint32_t status_runtime = 0x1U;
	constexpr uint32_t status_ban_read_database = 0x2U;
	constexpr uint32_t status_ban_write_database = 0x4U;

	extern const std::string nullstr;

	extern std::mutex lock_write;
	extern sqlite3* connect;

	bool init_database(std::string _game_db, std::string _user_db);
	bool init_database_struct();
	
	/*
	** 读取程序状态
	** @param _bit 状态类型
	** @return 返回获取到的程序状态 */
	inline bool read_status(uint32_t _bit)
	{
		return (app_status & _bit);
	}

	/*
	** 修改程序状态
	** @param _bit 状态类型
	** @param _new_status 新的状态 */
	inline void set_status(uint32_t _bit, bool _new_status)
	{
		if(_new_status)
		{
			app_status |= _bit;
		}
		else
		{
			app_status &= ~_bit;
		}
	
	}
}

