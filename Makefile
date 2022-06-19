CC = clang
LD = clang
LEX = flex

CFLAGS = -Wall
LDFLAGS =

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
LEXFILE = src/tab

.PHONY: all clean

all: bin/launchtab

bin/launchtab: $(LEXFILE).yy.o $(OBJ)
	mkdir -p bin
	$(LD) -o $@ $(LDFLAGS) $^

$(LEXFILE).yy.o: $(LEXFILE).yy.c
	$(CC) -o $@ -c $<

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

$(LEXFILE).yy.c: $(LEXFILE).l
	$(LEX) -o $@ --header-file=$(LEXFILE).yy.h --yylineno $<

clean:
	rm -f $(OBJ) $(LEXFILE).yy.* bin/*
	rm -df bin
