#include <iostream>
#include "src/core/user_attribute.h"

int main()
{
    mpdb::init_database("data/game.db", "data/user.db");
	mpdb::init_database_struct();
    
    mplog::message("shell", "debug") << mpdb::read_user_attribute_text(1, "name", "null") << mplog::push;
    
    system("pause");
    return 0;
}
