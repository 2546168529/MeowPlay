#include <iostream>
#include "src/core/player_attribute.h"
#include "src/util/math_exp.h"

int main()
{
    mp::init_database("data/game.db", "data/player_data.db");

    std::cout << (mp::write_player_attribute_text(2, "name", "Ao2")?"д��ɹ�":"д��ʧ��") << std::endl;
    
    std::cout << mp::read_player_attribute_text(2, "name") << std::endl;

    system("pause");
    return 0;
}
