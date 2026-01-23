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

# -------------------------
# Source files
# -------------------------
ALL_SRC_FILES := $(shell find $(SRC_DIR) -name "*.cpp")
ALL_TEST_SRC_FILES := $(shell find $(TEST_DIR) -name "*.cpp")

NESTEST_SRC := $(TEST_DIR)/nestest_runner.cpp
OTHER_TEST_SRCS := $(filter-out $(NESTEST_SRC),$(ALL_TEST_SRC_FILES))

LIB_SRC_FILES := $(filter-out $(SRC_DIR)/emulator_main.cpp,$(ALL_SRC_FILES))
GTEST_SRC_FILES := $(GTEST_DIR)/src/gtest-all.cc

LIB_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(LIB_SRC_FILES))
GTEST_OBJS := $(patsubst %.cc,$(OBJ_DIR)/%.o,$(GTEST_SRC_FILES))

OTHER_TEST_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(OTHER_TEST_SRCS))
NESTEST_OBJ := $(OBJ_DIR)/$(NESTEST_SRC:.cpp=.o)
MAIN_OBJ := $(OBJ_DIR)/$(SRC_DIR)/emulator_main.o

# Test executable names (strip path & extension)
EMULATOR_BIN := $(BIN_DIR)/nes_emulator
NES_TEST_BIN := $(TEST_BIN_DIR)/nes_test
CPU_TEST_BIN := $(TEST_BIN_DIR)/cpu_test

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
tests: $(CPU_TEST_BIN) $(NES_TEST_BIN)

$(CPU_TEST_BIN): $(OTHER_TEST_OBJS) $(LIB_OBJS) $(GTEST_OBJS)
	@mkdir -p $(TEST_BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(NES_TEST_BIN): $(NESTEST_OBJ) $(LIB_OBJS) $(GTEST_OBJS)
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
	@echo "-------------------------"
	@echo "Running CPU Tests"
	$(CPU_TEST_BIN)
	
	@echo "-------------------------"
	@echo "Running NES Tests"
	@cp -v $(TEST_DIR)/nestest.nes $(TEST_BIN_DIR)/
	$(NES_TEST_BIN) $(TEST_BIN_DIR)/nestest.nes
	
	@echo "-------------------------"
	@echo "All tests passed."

# -------------------------
# Clean
# -------------------------
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all tests run-tests clean
