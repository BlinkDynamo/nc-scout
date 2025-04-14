#----------------------------------------------------------------------------------------------#
# Variables
#----------------------------------------------------------------------------------------------#
# Compiler + flags to use. To omit debug flags, put a # to the right of the equals sign.
CC = gcc
CFLAGS_CORE     = -std=c99 -pedantic -Iinclude -O2
CFLAGS_WARNINGS = -Wall -Wextra -Werror
CFLAGS_DEBUG    = -g -ggdb3

# Installation prefix.
PREFIX ?= /usr/local

# Important directories.
BUILD_DIR = build
TESTS_DIR = $(BUILD_DIR)/tests
SRC_DIR = src
BIN_DIR = $(PREFIX)/bin

# Important files.
SRCS = src/main.c src/validate.c src/naming.c src/search.c src/analyze.c src/common.c
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
EXEC = $(BUILD_DIR)/nc-scout
TESTS_EXEC = tests/init.sh && tests/build.sh && tests/check.sh

# Exports. Used by scripts in tests/.
export BUILD_DIR := $(BUILD_DIR)
export TESTS_DIR := $(TESTS_DIR)

#----------------------------------------------------------------------------------------------#
# Dependency Tree 
#----------------------------------------------------------------------------------------------#
.PHONY: all check install uninstall clean

all: $(BUILD_DIR) $(EXEC)

check: $(BUILD_DIR) $(TESTS_DIR) $(EXEC)
	$(TESTS_EXEC)

install: $(EXEC)
	install -d $(DESTDIR)$(BIN_DIR)
	install -m 755 $(EXEC) $(DESTDIR)$(BIN_DIR)

uninstall:
	rm -f $(DESTDIR)$(BIN_DIR)/$(notdir $(EXEC))

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

$(BUILD_DIR)/$(SRC_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS_CORE) $(CFLAGS_WARNINGS) $(CFLAGS_DEBUG) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) 

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)

$(TESTS_DIR):
	mkdir -p $(TESTS_DIR)
