#Makefile

CC = g++
CFLAGS = -std=c++11

object = shell.o sqlite3.o basic.o user_attribute.o log.o math_exp.o

build: $(object)
	$(CC) $(CFLAGS) $(object) -o shell

shell.o: shell.cpp
	$(CC) $(CFLAGS) -c shell.cpp

basic.o: src/core/basic.cpp
	$(CC) $(CFLAGS) -c src/core/basic.cpp

log.o: src/util/log.cpp
	$(CC) $(CFLAGS) -c src/util/log.cpp

math_exp.o: src/util/math_exp.cpp
	$(CC) $(CFLAGS) -c src/util/math_exp.cpp

user_attribute.o: src/core/user_attribute.cpp
	$(CC) $(CFLAGS) -c src/core/user_attribute.cpp

sqlite3.o: src/sqlite/sqlite3.c
	gcc -c src/sqlite/sqlite3.c
