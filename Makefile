CC = clang
LD = clang
LEX = flex

CFLAGS := $(CFLAGS) -Wall
LDFLAGS := $(LDFLAGS)

LEXFILE = src/tab
SRC = $(wildcard src/*.c) $(LEXFILE).yy.c
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

.PHONY: all clean

all: bin/launchtab

bin/launchtab: $(OBJ)
	mkdir -p bin
	$(LD) -o $@ $(LDFLAGS) $^

-include $(DEP)

%.o: %.c Makefile
	$(CC) -o $@ $(CFLAGS) -MMD -MP -c $<

$(LEXFILE).yy.o: $(LEXFILE).yy.c Makefile
	$(CC) -o $@ -MMD -MP -c $<

$(LEXFILE).yy.c: $(LEXFILE).l
	$(LEX) -o $@ --header-file=$(LEXFILE).yy.h --yylineno $<

clean:
	rm -f $(OBJ) $(DEP) bin/*
	rm -df bin
