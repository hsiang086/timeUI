CC := gcc
CFLAGS := -Wall -Wextra -pedantic -std=c99
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
LIB_DIR := lib
BIN := fishtimeui

INCLUDES := -I$(LIB_DIR) -I$(SRC_DIR)
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET := $(BIN_DIR)/$(BIN)

.PHONY: all clean run compiledb

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

run: $(TARGET)
	@./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

compiledb:
	bear -- $(MAKE) clean all