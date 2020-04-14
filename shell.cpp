#include <iostream>
#include "core/user_attribute.h"

int main()
{
    mpdb::init_database("data/game.db", "data/user.db");
	mpdb::init_database_struct();
    
    std::cout << mpdb::read_user_attribute_text(1, "name", "bitch") << std::endl;
    
    system("pause");
    return 0;
}
