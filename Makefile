CC = clang
LD = clang
LEX = flex

CFLAGS := $(CFLAGS) -Wall
LDFLAGS :=

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)
LEXFILE = src/tab

.PHONY: all clean

all: bin/launchtab

bin/launchtab: $(LEXFILE).yy.o $(OBJ)
	mkdir -p bin
	$(LD) -o $@ $(LDFLAGS) $^

-include $(DEP)

%.o: %.c Makefile
	$(CC) -o $@ $(CFLAGS) -MMD -MP -c $<

$(LEXFILE).yy.o: $(LEXFILE).yy.c
	$(CC) -o $@ -c $<

$(LEXFILE).yy.c: $(LEXFILE).l src/launchtab.h
	$(LEX) -o $@ --header-file=$(LEXFILE).yy.h --yylineno $<

clean:
	rm -f $(OBJ) $(DEP) bin/*
	rm -df bin
