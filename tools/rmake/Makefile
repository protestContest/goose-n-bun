TARGET = rmake

CC := gcc

INC_DIR := inc
SRC_DIR := src
BUILD_DIR := build

SRC_C := $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/**/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.c.o,$(SRC_C))

IFLAGS := -I$(INC_DIR) -include ../../inc/prefix.h
# CFLAGS := -Wall -O3
CFLAGS := -O0 -g -fsanitize=address -fno-omit-frame-pointer $(IFLAGS)
LDFLAGS =

.PHONY: all clean

all: $(TARGET)

$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	@echo "  CC      $<"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	@echo "  LD      $@"
	@mkdir -p $(@D)
	@$(CC) -o $@ $(OBJS) $(LDFLAGS) $(CFLAGS)

res-extract: res_extract.c
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "  CLEAN   $(TARGET) res-extract $(BUILD_DIR)"
	@rm -rf $(TARGET) res-extract $(BUILD_DIR)
