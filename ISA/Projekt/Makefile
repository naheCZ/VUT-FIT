# Projekt: POP3 Server
# Autor:   Roman Nahalka, xnahal01@stud.fit.vutbr.cz
# Datum:   24.10.2017

CC = g++
CFLAGS = -std=c++11 -Wall -pedantic -Wextra
BIN = popser
SOURCE = main.cpp
REST = arguments.cpp error.cpp server.cpp user.cpp md5.cpp
FILES = main.cpp arguments.cpp arguments.h error.cpp error.h server.cpp server.h user.cpp user.h md5.cpp md5.h Makefile manual.pdf README
OPTIONS = -pthread -lpthread
LOGIN = xnahal01
RM = rm -f

$(BIN): $(SOURCE)
	$(CC) $(CFLAGS) -o $(BIN) $(SOURCE) $(REST) -o $(BIN) $(OPTIONS)

clean:
	$(RM) *.o $(BIN)

tar:
	tar -cf $(LOGIN).tar $(FILES)
