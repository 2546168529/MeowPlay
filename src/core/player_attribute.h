#pragma once
#include "basic.h"
#include <string>
#include <functional>

namespace mp {
    bool read_player_attribute(int64_t _player_id, std::string& _properties, std::function<void(sqlite3_stmt*, int)>);
    int32_t read_player_attribute_int32(int64_t _player_id, std::string _properties, int32_t _default = 0);
    int64_t read_player_attribute_int64(int64_t _player_id, std::string _properties, int64_t _default = 0);
    double read_player_attribute_double(int64_t _player_id, std::string _properties, double _default = 0);
    std::string read_player_attribute_text(int64_t _player_id, std::string _properties, std::string _default = nullstr);

    bool write_player_attribute(int64_t _player_id, std::string& _properties, std::function<void(sqlite3_stmt*, int)>);
    bool write_player_attribute_int32(int64_t _player_id, std::string _properties, int32_t _data);
    bool write_player_attribute_int64(int64_t _player_id, std::string _properties, int64_t _data);
    bool write_player_attribute_double(int64_t _player_id, std::string _properties, double _data);
    bool write_player_attribute_text(int64_t _player_id, std::string _properties, std::string _data);

}