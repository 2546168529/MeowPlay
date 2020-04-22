#pragma one
#include "basic.h"
#include <string>
#include "../json/json11.hpp"

namespace mp
{

struct BaseItemsInfo
{
    uint32_t item_base_id;
    std::string item_name;
    int32_t item_type;
    bool uniqueness;
    std::string base_info;
};

mp::Status add_player_items(int64_t _player_id, uint32_t _item_base_id, int64_t _amount, int32_t _position, int32_t _position_weight, int64_t _time_limit);
mp::Status read_base_items(uint32_t _item_base_id, mp::BaseItemsInfo &_read_result);
uint32_t *get_items_uuid_base(uint32_t _item_base_id, int64_t _amount);
mp::Status update_player_items(int64_t _player_id, int64_t _item_uuid, int32_t _amount, int32_t _position, int32_t _position_weight, int64_t _other);
mp::Status regist_items(int64_t _item_uuid, uint32_t _item_base_id, int64_t _time_limit, json11::Json &_base_info);

}; // namespace mp
