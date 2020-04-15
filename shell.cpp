#include <iostream>
#include "src/core/user_attribute.h"
#include "src/util/math_exp.h"

int main()
{
    mp::init_database("data/game.db", "data/user.db");
	mp::init_database_struct();
    
    mp::log("shell", "debug") << mp::read_user_attribute_text(1, "name", "null") << mp::log::push;
    
    mp::log("shell", "debug") << calculate_expr("5*19") << mp::log::push;
    mp::log("shell", "debug") << calculate_expr("3040/4") << mp::log::push;
    mp::log("shell", "debug") << calculate_expr("2*((1+1)*(1+3)+2)/1*2") << mp::log::push;
    mp::log("shell", "debug") << calculate_expr("10 + 2 ^ (4 + 6)") << mp::log::push;
    mp::log("shell", "debug") << calculate_expr("10+(2/1*4)+4-6*(5+7)") << mp::log::push;
    mp::log("shell", "debug") << calculate_expr("7+8*3*(5+7*2/(2+1)*8/2+(6+23*(9+4)))") << mp::log::push;
    mp::log("shell", "debug") << calculate_expr("-7+8*3*(5+7*2/(2+1)*8/2+(6+23*(9+4)))*6/(1+2)*12+1") << mp::log::push;

    system("pause");
    return 0;
}
