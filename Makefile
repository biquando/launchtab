CC = clang
LD = clang
LEX = flex

CFLAGS = -Wall
LDFLAGS =

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: bin/launchtab

bin/launchtab: tab.yy.o $(OBJ)
	mkdir -p bin
	$(LD) -o $@ $(LDFLAGS) $^
	cp ltreload bin

tab.yy.o: tab.yy.c
	$(CC) -o $@ -c $<

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

tab.yy.c: tab.l
	$(LEX) -o $@ --header-file=tab.yy.h --yylineno $<

clean:
	rm -f $(OBJ) tab.yy.* bin/*
	rm -df bin
