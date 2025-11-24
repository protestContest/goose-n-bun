NAME := charsheet
GAME_TITLE := "Char Sheet"
GAME_CODE := 00

DEFINES :=
INCLUDES := inc
LIBS := -lc

CC := arm-none-eabi-gcc
OBJDUMP := arm-none-eabi-objdump
OBJCOPY := arm-none-eabi-objcopy

SRC_DIR := src
BUILD_DIR := build
DIST_DIR := dist

ELF := $(BUILD_DIR)/$(NAME).elf
DUMP := $(BUILD_DIR)/$(NAME).dump
ROM := $(DIST_DIR)/$(NAME).gba
MAP := $(BUILD_DIR)/$(NAME).map
GBAFIX := gbafix/gbafix

SRC_S := $(wildcard $(SRC_DIR)/*.s $(SRC_DIR)/**/*.s)
SRC_C := $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/**/*.c)

DEFINES += -D__GBA__
ARCH := -mcpu=arm7tdmi -mtune=arm7tdmi

IFLAGS := -I$(INCLUDES) -include univ/prefix.h
WFLAGS := -Wall -Wextra -Werror

ASFLAGS += -x assembler-with-cpp $(DEFINES) $(ARCH) -mthumb -mthumb-interwork -ffunction-sections -fdata-sections
CFLAGS += -std=c99 $(WFLAGS) $(DEFINES) $(ARCH) -mthumb -mthumb-interwork $(IFLAGS) -O3 -ffunction-sections -fdata-sections
LDFLAGS := -mthumb -mthumb-interwork -Wl,-Map,$(MAP) -Wl,--gc-sections -specs=nano.specs -T $(SRC_DIR)/sys/gba_cart.ld -Wl,--start-group $(LIBS) -Wl,--end-group

OBJS := $(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/%.s.o,$(SRC_S)) $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.c.o,$(SRC_C))
DEPS := $(OBJS:.o=.d)

$(BUILD_DIR)/%.s.o: $(SRC_DIR)/%.s
	@echo "  AS      $<"
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -MMD -MP -c -o $@ $<

$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	@echo "  CC      $<"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

.PHONY: all clean dump

all: $(ROM)

$(GBAFIX):
	cd gbafix && make

$(ELF): $(OBJS)
	@echo "  LD      $@"
	@mkdir -p $(@D)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(ROM): $(ELF) $(GBAFIX)
	@echo "  OBJCOPY $<"
	@mkdir -p $(@D)
	$(OBJCOPY) -O binary $< $@
	@echo "  GBAFIX  $@"
	$(GBAFIX) $@ -t$(GAME_TITLE) -c$(GAME_CODE)

$(DUMP): $(ELF)
	@echo "  OBJDUMP $@"
	$(OBJDUMP) -h -C -S $< > $@

dump: $(DUMP)

clean:
	@echo "  CLEAN"
	rm -rf $(BUILD_DIR) $(DIST_DIR)
	cd gbafix && make clean
