#include <iostream>
#include "src/core/user_attribute.h"

int main()
{
    mp::init_database("data/game.db", "data/user.db");
	mp::init_database_struct();
    
    mp::log("shell", "debug") << mp::read_user_attribute_text(1, "name", "null") << mp::log::push;
    
    system("pause");
    return 0;
}
