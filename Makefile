#----------------------------------------------------------------------------------------------#
# Variables
#----------------------------------------------------------------------------------------------#
# Compiler + flags to use. To omit debug flags, put a # to the right of the equals sign.
CC = gcc
CFLAGS_CORE     = -std=c99 -pedantic -Iinclude -O3
CFLAGS_WARNINGS = -Wall -Wextra -Werror

# Installation prefix.
PREFIX ?= /usr/local

# Important directories.
BUILD_DIR = build
TESTS_DIR = $(BUILD_DIR)/tests
SRC_DIR = src
BIN_DIR = $(PREFIX)/bin
MAN_DIR = $(PREFIX)/share/man/man1
COMPLETION_DIR = /etc/bash_completion.d

# Important files.
SRCS = src/main.c src/validate.c src/naming.c src/search.c src/analyze.c src/common.c
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
EXEC = $(BUILD_DIR)/nctool
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
	install -Dm 755 $(EXEC) $(DESTDIR)$(BIN_DIR)
	install -Dm 644 man/nctool.1 $(DESTDIR)$(MAN_DIR)/nctool.1
	install -m 644 completions/nctool $(COMPLETION_DIR)/nctool

uninstall:
	rm -f $(DESTDIR)$(BIN_DIR)/$(notdir $(EXEC))
	rm -f $(DESTDIR)$(MAN_DIR)/nctool.1
	rm -f $(COMPLETION_DIR)/nctool

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

$(BUILD_DIR)/$(SRC_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS_CORE) $(CFLAGS_WARNINGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) 

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)

$(TESTS_DIR):
	mkdir -p $(TESTS_DIR)
