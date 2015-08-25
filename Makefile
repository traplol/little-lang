CC:=clang
SRC_DIR:= src
OBJ_DIR:= obj
BIN_DIR:= bin
INCLUDES:= -Iinclude
CFLAGS:= -O0 -Werror -Wall -pedantic -pedantic-errors -Wextra -g -std=c89 $(INCLUDES)
LDFLAGS:=
SOURCES:= $(wildcard $(SRC_DIR)/*.c)
OBJECTS:= $(addprefix $(OBJ_DIR)/,$(notdir $(SOURCES:.c=.o)))
EXECUTABLE:= $(BIN_DIR)/little-lang

.PHONY: exe clean

exe: $(OBJ_DIR) $(BIN_DIR) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) 
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean:
	rm -f $(EXECUTABLE)
	rm -f $(OBJECTS)
