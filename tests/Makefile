CC:= clang
SOURCES:= $(wildcard src/*test.c)
TESTS:= $(addprefix bin/,$(notdir $(SOURCES:.c=)))
INCLUDES:= -I../include
CFLAGS:= -O0 -Werror -Wall -pedantic -pedantic-errors -Wextra -g -std=c99 $(INCLUDES)

all: bin $(TESTS)

bin/%test: ../src/*.c src/*.c
	$(CC) $(CFLAGS) $(addprefix src/,$(notdir $@)).c -o $@

bin:
	@mkdir -p $@

clean:
	rm -f bin/*
