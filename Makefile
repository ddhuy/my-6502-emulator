CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -I./src

# -------------------------
# Build mode
# -------------------------
BUILD ?= debug

ifeq ($(BUILD),debug)
    CXXFLAGS += -g -O0 -DDEBUG \
                -fsanitize=address,undefined \
                -fno-omit-frame-pointer
else ifeq ($(BUILD),release)
    CXXFLAGS += -O2 -DNDEBUG
endif

# -------------------------
# Directories
# -------------------------
SRC_DIR    := src
TEST_DIR   := tests
BUILD_DIR  := build
OBJ_DIR    := $(BUILD_DIR)/obj
BIN_DIR    := $(BUILD_DIR)/bin
TEST_BIN_DIR := $(BIN_DIR)/tests

EMULATOR_BIN := $(BIN_DIR)/emulator

# -------------------------
# Source files
# -------------------------
ALL_SRC_FILES := $(shell find $(SRC_DIR) -name "*.cpp")
LIB_SRC_FILES := $(filter-out $(SRC_DIR)/main.cpp,$(ALL_SRC_FILES))
TEST_SRC_FILES := $(shell find $(TEST_DIR) -name "*.cpp")

LIB_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(LIB_SRC_FILES))
MAIN_OBJ := $(OBJ_DIR)/$(SRC_DIR)/main.o

TEST_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_SRC_FILES))

# Test executable names (strip path & extension)
TEST_BINS := $(patsubst $(TEST_DIR)/%.cpp,$(TEST_BIN_DIR)/%,$(TEST_SRC_FILES))

# -------------------------
# Default target
# -------------------------
all: $(EMULATOR_BIN)

# -------------------------
# Emulator
# -------------------------
$(EMULATOR_BIN): $(LIB_OBJS) $(MAIN_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# -------------------------
# Tests
# -------------------------
tests: $(TEST_BINS)

# Rule: one test source -> one test binary
$(TEST_BIN_DIR)/%: $(OBJ_DIR)/$(TEST_DIR)/%.o $(LIB_OBJS)
	@mkdir -p $(TEST_BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# -------------------------
# Object compilation
# -------------------------
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------------------------
# Run all tests
# -------------------------
run-tests: tests
	@set -e; \
	for t in $(TEST_BINS); do \
	    echo "Running $$t"; \
	    $$t; \
	done; \
	echo "All tests passed."

# -------------------------
# Clean
# -------------------------
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all tests run-tests clean
