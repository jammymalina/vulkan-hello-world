CC = gcc
STD = -std=c23

BUILD_TYPE ?= debug

CFLAGS_BASE = -Wall -Wextra $(STD) -DVK_NO_PROTOTYPES
CFLAGS_DEBUG = $(CFLAGS_BASE) -g -DDEBUG -O0
CFLAGS_PROD = $(CFLAGS_BASE) -O3 -DNDEBUG

LFLAGS_BASE = -lm -lSDL2
LFLAGS_DEBUG = $(LFLAGS_BASE)
LFLAGS_PROD = $(LFLAGS_BASE) -flto -O3 -march=native

ifeq ($(BUILD_TYPE),debug)
	CFLAGS = $(CFLAGS_DEBUG)
	LFLAGS = $(LFLAGS_DEBUG)
	TARGET_DIR = debug
else ifeq ($(BUILD_TYPE),prod)
	CFLAGS = $(CFLAGS_PROD)
	LFLAGS = $(LFLAGS_PROD)
	TARGET_DIR = prod
else
	$(error Invalid BUILD_TYPE: $(BUILD_TYPE). Use 'debug' or 'prod')
endif

TARGET_DIR := dist/$(TARGET_DIR)
TARGET = $(TARGET_DIR)/hello

SRCS := $(wildcard src/*.c src/*/*.c)
OBJS = $(SRCS:src/%.c=$(TARGET_DIR)/%.o)

INC_DIRS := src $(wildcard src/*/ src/*/*/)
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

CFLAGS += $(INC_FLAGS)

all: $(TARGET)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LFLAGS) $(OBJS) -o $(TARGET)

$(TARGET_DIR)/%.o: src/%.c | $(TARGET_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf dist

mem-check: $(TARGET)
	@valgrind --leak-check=yes $(TARGET)

rebuild: clean all

config:
	@echo "Build type: $(BUILD_TYPE)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "Source files:"
	@echo $(SRCS)
	@echo "Object files:"
	@echo $(OBJS)

.PHONY: all clean rebuild config mem-check
