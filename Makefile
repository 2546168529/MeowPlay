#Makefile

CC = g++
CFLAGS = -std=c++11

build: shell.cpp sqlite3.o basic.o user_attribute.o 
	$(CC) $(CFLAGS) shell.cpp basic.o user_attribute.o sqlite3.o -o shell

basic.o: core/basic.cpp
	$(CC) $(CFLAGS) -c core/basic.cpp

user_attribute.o: core/user_attribute.cpp
	$(CC) $(CFLAGS) -c core/user_attribute.cpp

sqlite3.o: sqlite/sqlite3.c
	gcc -c ./sqlite/sqlite3.c

clear:
	rm *.o