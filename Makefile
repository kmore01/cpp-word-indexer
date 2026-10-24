# Alternativa ligera a CMake para compilar sin dependencias externas.
CXX ?= g++
CPPFLAGS ?= -Iinclude
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

BUILD_DIR := build
TARGET := $(BUILD_DIR)/word_indexer
TEST_BIN := $(BUILD_DIR)/trie_tests
SOURCES := src/main.cpp src/NodoTrie.cpp src/NodoRadixTrie.cpp
OBJECTS := $(SOURCES:src/%.cpp=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	$(CXX) $(OBJECTS) -o $@

$(TEST_BIN): tests/test_tries.cpp src/NodoTrie.cpp src/NodoRadixTrie.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean
