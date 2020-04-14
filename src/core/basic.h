#pragma once

#include <mutex>
#include <inttypes.h>
#include <string>

#include "../util/log.h"
#include "../sqlite/sqlite3.h"

namespace mpdb {
	extern std::mutex lock;
	extern sqlite3* connect;

	bool init_database(std::string _game_db, std::string _user_db);
	bool init_database_struct();
}

