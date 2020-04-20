#pragma one
#include "basic.h"
#include <string>


namespace mp {

    struct BaseItemsInfo
    {
        uint32_t item_base_id;
        std::string item_name;
        int32_t item_type;
        bool uniqueness;
        std::string base_info;
    };

    mp::Status add_player_items(int64_t _player_id, uint32_t _item_base_id, int64_t _amount, int32_t _position, int32_t _position_weight, int64_t _time_limit);
};
