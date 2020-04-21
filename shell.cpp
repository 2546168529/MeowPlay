#include <iostream>
#include "src/core/player_attribute.h"
#include "src/util/math_exp.h"
#include "src/core/items_system.h"

int main()
{
    mp::init_database("data/basic.db", "data/player.db");

    // std::cout << (mp::write_player_attribute_text(2, "name", "测试")==mp::status_ok?"写入成功":"写入失败") << std::endl;
    
    // std::cout << mp::read_player_attribute_text(2, "name", "") << std::endl;
    std::cout << mp::add_player_items(1, 1, 3, 1, 1, 10) << std::endl;

    system("pause");
    return 0;
}
