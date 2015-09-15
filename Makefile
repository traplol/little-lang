CC:=clang
SRC_DIR:= src
HELPERS_DIR:= helpers
RUNTIME_DIR:= runtime
OBJ_DIR:= obj
BIN_DIR:= bin
INCLUDES:= -Iinclude -I.
#CFLAGS:= -O0 -D_GNU_SOURCE -Werror -Wall -pedantic -pedantic-errors -Wextra -g -std=c99 $(INCLUDES)
CFLAGS:= -O0 -g -std=c99 -D_GNU_SOURCE $(INCLUDES)
LDFLAGS:= -lm
SOURCES:= $(wildcard $(SRC_DIR)/*.c)
SOURCES+= $(wildcard $(HELPERS_DIR)/*.c)
SOURCES+= $(wildcard $(RUNTIME_DIR)/*.c)
OBJECTS:= $(addprefix $(OBJ_DIR)/,$(notdir $(SOURCES:.c=.o)))
EXECUTABLE:= $(BIN_DIR)/little-lang

.PHONY: exe clean echo-vars

exe: $(OBJ_DIR) $(BIN_DIR) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(HELPERS_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(RUNTIME_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean:
	rm -f $(EXECUTABLE)
	rm -f $(OBJECTS)

echo-vars:
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
