#include <iostream>
#include "src/core/user_attribute.h"
#include "src/util/math_exp.h"

int main()
{
    mp::init_database("data/game.db", "data/user.db");
	mp::init_database_struct();

    std::cout << (mp::write_user_attribute_text(2, "name", "Ao2")?"写入成功":"写入失败") << std::endl;
    
    std::cout << mp::read_user_attribute_text(2, "name") << std::endl;

    system("pause");
    return 0;
}
