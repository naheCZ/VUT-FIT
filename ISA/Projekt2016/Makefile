# Projekt: Analyzator sitoveho provozu
# Autor:   Roman Nahalka
# Datum:   4.10.2016


CC = g++
CFLAGS = -Wall -pedantic -Wextra
BIN = analyzer
SOURCE = analyzer.cpp
RM = rm -f

$(BIN): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(BIN)

clean:
	$(RM) *.o $(BIN)
