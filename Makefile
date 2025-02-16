CC = gcc
STD = -std=c23

BUILD_TYPE ?= debug

CFLAGS_BASE = -Wall -Wextra $(STD)

CFLAGS_DEBUG = $(CFLAGS_BASE) -g -DDEBUG -O0
CFLAGS_PROD = $(CFLAGS_BASE) -O3 -DNDEBUG

ifeq ($(BUILD_TYPE),debug)
    CFLAGS = $(CFLAGS_DEBUG)
    TARGET_DIR = debug
else ifeq ($(BUILD_TYPE),prod)
    CFLAGS = $(CFLAGS_PROD)
    TARGET_DIR = prod
else
    $(error Invalid BUILD_TYPE: $(BUILD_TYPE). Use 'debug' or 'prod')
endif

TARGET = dist/$(TARGET_DIR)/hello

SRCS := $(wildcard src/*.c)
OBJS = $(SRCS:src/%.c=$(TARGET_DIR)/%.o)

INC_DIRS := src $(wildcard src/*/ src/*/*/)
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

CFLAGS += $(INC_FLAGS)

all: $(TARGET)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) -o $(TARGET)

$(TARGET_DIR)/%.o: src/%.c | $(TARGET_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf debug prod

rebuild: clean all

config:
	@echo "Build type: $(BUILD_TYPE)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "Source files:"
	@echo $(SRCS)
	@echo "Object files:"
	@echo $(OBJS)

.PHONY: all clean rebuild config
