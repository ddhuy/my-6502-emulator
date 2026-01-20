CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -I./src \
             -I./3rd_party/googletest/googletest/include/
LDFLAGS   := -lpthread

# -------------------------
# Build mode
# -------------------------
BUILD ?= debug

ifeq ($(BUILD),debug)
    CXXFLAGS += -O0 -g -DDEBUG \
                -fsanitize=address,undefined \
                -fno-omit-frame-pointer
else ifeq ($(BUILD),release)
    CXXFLAGS += -O3 -flto -march=native -DNDEBUG
endif

# -------------------------
# Directories
# -------------------------
SRC_DIR    := src
TEST_DIR   := tests
BUILD_DIR  := build
OBJ_DIR    := $(BUILD_DIR)/obj
BIN_DIR    := $(BUILD_DIR)/bin

GTEST_DIR := 3rd_party/googletest/googletest

TEST_BIN_DIR := $(BIN_DIR)/tests
EMULATOR_BIN := $(BIN_DIR)/emulator

# -------------------------
# Source files
# -------------------------
ALL_SRC_FILES := $(shell find $(SRC_DIR) -name "*.cpp")
LIB_SRC_FILES := $(filter-out $(SRC_DIR)/main.cpp,$(ALL_SRC_FILES))
TEST_SRC_FILES := $(shell find $(TEST_DIR) -name "*.cpp")
GTEST_SRC_FILES := $(GTEST_DIR)/src/gtest-all.cc

LIB_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(LIB_SRC_FILES))
TEST_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_SRC_FILES))
GTEST_OBJS := $(patsubst %.cc,$(OBJ_DIR)/%.o,$(GTEST_SRC_FILES))

# Test executable names (strip path & extension)
MAIN_OBJ := $(OBJ_DIR)/$(SRC_DIR)/main.o
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
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# -------------------------
# Tests
# -------------------------
tests: $(TEST_BINS)

# Rule: one test source -> one test binary
$(TEST_BIN_DIR)/%: $(OBJ_DIR)/$(TEST_DIR)/%.o $(LIB_OBJS) $(GTEST_OBJS)
	@mkdir -p $(TEST_BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# -------------------------
# Object compilation
# -------------------------
$(GTEST_OBJS): $(GTEST_SRC_FILES)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------------------------
# Run all tests
# -------------------------
run-tests: tests
	@cp -vr $(TEST_DIR)/nestest $(BIN_DIR)/tests/nestest
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
