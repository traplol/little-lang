CC:= clang
INCLUDES:= -I../include
CFLAGS:= -O0 -Werror -Wall -pedantic -pedantic-errors -Wextra -g -std=c99 $(INCLUDES)

all: symbol_table_test

out: bin obj

symbol_table_test: out
	$(CC) $(CFLAGS) $@.c -o bin/$@

obj:
	@mkdir -p $@

bin:
	@mkdir -p $@

clean:
	rm obj/*
	rm bin/*
