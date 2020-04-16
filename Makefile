#Makefile

CC = g++
CFLAGS = -std=c++17

object = obj/shell.o obj/sqlite3.o obj/basic.o obj/player_attribute.o obj/log.o obj/math_exp.o obj/utility_str.o obj/database.o

build: $(object)
	$(CC) $(CFLAGS) $(object) -o shell

obj/shell.o: shell.cpp
	$(CC) $(CFLAGS) -c shell.cpp -o $@

obj/basic.o: src/core/basic.cpp
	$(CC) $(CFLAGS) -c src/core/basic.cpp -o $@

obj/log.o: src/util/log.cpp
	$(CC) $(CFLAGS) -c src/util/log.cpp -o $@

obj/math_exp.o: src/util/math_exp.cpp
	$(CC) $(CFLAGS) -c src/util/math_exp.cpp -o $@

obj/utility_str.o: src/util/utility_str.cpp
	$(CC) $(CFLAGS) -c src/util/utility_str.cpp -o $@

obj/player_attribute.o: src/core/player_attribute.cpp
	$(CC) $(CFLAGS) -c src/core/player_attribute.cpp -o $@

obj/sqlite3.o: src/sqlite/sqlite3.c
	gcc -c src/sqlite/sqlite3.c -o $@

obj/database.o: src/sqlite/database.cpp
	$(CC) $(CFLAGS) -c src/sqlite/database.cpp -o $@