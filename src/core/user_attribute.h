#pragma once
#include "basic.h"
#include <string>

namespace mp {
    int32_t read_user_attribute_int32(int64_t _user, std::string _properties, int32_t _default = 0);
    int64_t read_user_attribute_int64(int64_t _user, std::string _properties, int64_t _default = 0);
    std::string read_user_attribute_text(int64_t _user, std::string _properties, const char* _default = nullptr);
}