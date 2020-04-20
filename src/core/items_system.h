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

};
