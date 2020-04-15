#pragma once
#include "basic.h"
#include <string>

namespace mp {
    bool read_user_attribute(int64_t _user, std::string _properties, void (*read_data)(sqlite3_stmt*, int));

    bool write_user_attribute(int64_t _user, std::string _properties, void (*bind_new_data)(sqlite3_stmt*, int));
}