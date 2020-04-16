#Makefile

CC = g++
CFLAGS = -std=c++17

object = obj/shell.o obj/sqlite3.o obj/basic.o obj/user_attribute.o obj/log.o obj/math_exp.o

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

obj/user_attribute.o: src/core/user_attribute.cpp
	$(CC) $(CFLAGS) -c src/core/user_attribute.cpp -o $@

obj/sqlite3.o: src/sqlite/sqlite3.c
	gcc -c src/sqlite/sqlite3.c -o $@
