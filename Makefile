# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -O3 -I./src

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN := webserver  # name of the final executable

# Source and object files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Default target
all: $(BIN)

# Link the final binary
$(BIN): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@

# Compile .c files to .o files in build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run the built binary
run: $(BIN)
	./$(BIN)

# Clean target
clean:
	rm -rf $(BUILD_DIR) $(BIN)

.PHONY: all clean run

