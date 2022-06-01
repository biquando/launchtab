CC = gcc
LD = gcc
LEX = flex

CFLAGS =
LDFLAGS =

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: launchtab

launchtab: tab.yy.o $(OBJ)
	mkdir -p bin
	$(LD) -o bin/$@ $(LDFLAGS) $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

tab.yy.c: tab.l
	$(LEX) -o $@ --header-file=tab.yy.h --yylineno $<

clean:
	rm -f $(OBJ) tab.yy.* bin/*
	rmdir bin
