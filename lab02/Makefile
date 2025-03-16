CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
AR = ar
ARFLAGS = rcs

SRC_DIR = src
BUILD_DIR = build

LIB_NAME = libcollatz.a
LIB_OBJ = $(BUILD_DIR)/collatz.o
MAIN_OBJ = $(BUILD_DIR)/main.o
TARGET = $(BUILD_DIR)/static_client

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB_OBJ): $(SRC_DIR)/collatz.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN_OBJ): $(SRC_DIR)/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(LIB_NAME): $(LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(TARGET): $(MAIN_OBJ) $(BUILD_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $^ -o $@

static_client: $(TARGET)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean