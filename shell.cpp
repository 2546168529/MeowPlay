#include <iostream>
#include "src/core/user_attribute.h"
#include "src/util/math_exp.h"

int main()
{
    mp::init_database("data/game.db", "data/user.db");
	mp::init_database_struct();

    std::cout << mp::write_user_attribute(1, "name", [](sqlite3_stmt* _stmt, int _index){
            sqlite3_bind_int(_stmt, _index, 1236);
        }) << std::endl;

    mp::read_user_attribute(1, "name", [](sqlite3_stmt* _stmt, int _index){
        mp::log("shell", "debug") << reinterpret_cast<const char*>(sqlite3_column_text(_stmt, _index)) << mp::log::push;
    });

    system("pause");
    return 0;
}
