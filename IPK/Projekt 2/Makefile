# Projekt: Projekt 2
# Autor:   Roman Nahalka
# Datum:   11.4.2016


CC = g++
CFLAGS = -Wall -pedantic -Wextra

all: server client

client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o client

server: server.cpp
	$(CC) $(CFLAGS) server.cpp -o server
