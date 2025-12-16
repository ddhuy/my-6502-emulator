# Default build type
BUILD ?= debug

# -------------------------
# Compiler and flags
# -------------------------
CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -Werror=return-type -I./src

ifeq ($(BUILD),debug)
    CXXFLAGS += -g -O0 -DDEBUG
    CXXFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
else ifeq ($(BUILD),release)
    CXXFLAGS += -O2 -DNDEBUG
endif

SRC_DIR   := src
TEST_DIR  := tests
BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
BIN_DIR   := $(BUILD_DIR)/bin

EMULATOR_BIN := $(BIN_DIR)/emulator
TEST_BIN     := $(BIN_DIR)/emulator_tests

# -------------------------
# Source files
# -------------------------
ALL_SRC_FILES := $(shell find $(SRC_DIR) -name "*.cpp")
LIB_SRC_FILES := $(filter-out $(SRC_DIR)/main.cpp,$(ALL_SRC_FILES))
TEST_FILES    := $(shell find $(TEST_DIR) -name "*.cpp")

LIB_OBJS  := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(LIB_SRC_FILES))
MAIN_OBJ  := $(OBJ_DIR)/$(SRC_DIR)/main.o
TEST_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_FILES))

# -------------------------
# Default target
# -------------------------
all: $(EMULATOR_BIN)

# -------------------------
# Emulator binary
# -------------------------
$(EMULATOR_BIN): $(LIB_OBJS) $(MAIN_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# -------------------------
# Test binary
# -------------------------
tests: $(TEST_BIN)

$(TEST_BIN): $(LIB_OBJS) $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# -------------------------
# Object compilation
# -------------------------
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------------------------
# Housekeeping
# -------------------------
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all tests clean
