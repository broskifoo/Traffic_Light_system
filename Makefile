CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude -g
LDFLAGS = 
TARGET = smart_traffic
SRC_DIR = src
SIM_DIR = simulator
TEST_DIR = tests
BUILD_DIR = build
INCLUDE_DIR = include

SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SIM_DIR)/*.c)
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:%.c=$(BUILD_DIR)/%.o)
TEST_TARGET = $(BUILD_DIR)/test_runner

.PHONY: all clean run test

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(filter-out $(BUILD_DIR)/src/main.o, $(OBJECTS)) $(TEST_OBJECTS) | $(BUILD_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)