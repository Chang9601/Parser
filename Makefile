CC=gcc
CFLAGS=-g -Wall -Wshadow -Wvla -Werror -pedantic
DEBUG=gdb
SRC_H=parser.h
SRC_C=parser.c main.c
EXECUTABLE=parser

$(EXECUTABLE):$(SRC_H) $(SRC_C)
	$(CC) -o parser $(CFLAGS) parser.c main.c

debug:
	$(DEBUG) parser

clean:
	rm ./$(EXECUTABLE)
