#pragma once

#include <mutex>
#include <inttypes.h>
#include <string>

#include "../util/log.h"
#include "../sqlite/sqlite3.h"

namespace mp {
	extern std::mutex lock_write;
	extern sqlite3* connect;
	extern const std::string nullstr;

	bool init_database(std::string _game_db, std::string _user_db);
	bool init_database_struct();
}

