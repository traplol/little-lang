CC:= clang
INCLUDES:= -I../include
CFLAGS:= -O0 -Werror -Wall -pedantic -pedantic-errors -Wextra -g -std=c99 $(INCLUDES)

all: table_tests

out: bin obj

table_tests: out
	$(CC) $(CFLAGS) $@.c -o bin/$@

obj:
	@mkdir -p $@

bin:
	@mkdir -p $@

clean:
	rm obj/*
	rm bin/*
