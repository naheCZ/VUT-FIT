CFLAGS = -std=c99 -Wall -pedantic -g
BIN = main
CC = gcc
RM = rm -f

main: main.c
	$(CC) $(CFLAGS) -o $(BIN) error.c ilist.c scaner.c parser.c str.c main.c ial.c interpret.c functions.c expressions.c  stack.c -o main

clean:
	$(RM) *.o $(BIN)

parser: parser.c
	$(CC) $(CFLAGS) -o parser parser.c stack.c error.c str.c scaner.c ial.c expressions.c functions.c ilist.c interpret.c

clean_parser:
	$(RM) *.o parser
