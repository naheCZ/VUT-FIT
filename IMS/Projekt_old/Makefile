# Projekt: Supermarket pro IMS
# Autor:   Roman Nahalka
# Datum:   24.11.2016


CC = g++
CFLAGS = -Wall -pedantic -Wextra
BIN = supermarket
SOURCE = supermarket.cpp
FILES = supermarket.cpp Makefile
LOGIN = xnahal01
RM = rm -f

$(BIN): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(BIN) -lsimlib -lm

clean:
	$(RM) *.o $(BIN) output.out

run:
	./$(BIN)
	cat output.out

tar:
	tar -cf $(LOGIN).tar $(FILES)
