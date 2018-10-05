# Projekt: Supermarket pro IMS
# Autor:   Roman Nahalka
# Datum:   30.11.2017


CC = g++
CFLAGS = -Wall -pedantic -Wextra -std=c++11
BIN = maslo
SOURCE = maslo.cpp
FILES = maslo.cpp Makefile
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
