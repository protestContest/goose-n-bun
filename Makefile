NAME := bun
GAME_TITLE := GOOSENBUN
GAME_CODE := ZACK

DEFINES :=
LIBS :=

CC := arm-none-eabi-gcc
OBJDUMP := arm-none-eabi-objdump
OBJCOPY := arm-none-eabi-objcopy

INC_DIR := inc
SRC_DIR := src
RES_DIR := res
BUILD_DIR := build
DIST_DIR := dist

ELF := $(BUILD_DIR)/$(NAME).elf
DUMP := $(BUILD_DIR)/$(NAME).dump
ROM := $(DIST_DIR)/$(NAME).gba
MAP := $(BUILD_DIR)/$(NAME).map
ROMFIX := tools/romfix/romfix
RMAKE := tools/rmake/rmake

RES_MANIFEST := res/Manifest

SRC_S := $(wildcard $(SRC_DIR)/*.s $(SRC_DIR)/**/*.s)
SRC_C := $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/**/*.c)
RES_R := $(wildcard $(RES_DIR)/*)

DEFINES += -D__GBA__
ARCH := -mcpu=arm7tdmi -mtune=arm7tdmi

IFLAGS := -I$(INC_DIR) -include prefix.h
WFLAGS := -Wall -Wextra -Werror -Wno-multichar -Wno-unused-parameter -Wno-main

ASFLAGS += -x assembler-with-cpp $(DEFINES) $(ARCH) -mthumb -mthumb-interwork -ffunction-sections -fdata-sections
CFLAGS += -std=c99 $(WFLAGS) $(DEFINES) $(ARCH) -mthumb -mthumb-interwork $(IFLAGS) -O3 -ffunction-sections -fdata-sections
LDFLAGS := -mthumb -mthumb-interwork -Wl,-Map,$(MAP) -Wl,--gc-sections -specs=nano.specs -T $(SRC_DIR)/sys/gba_cart.ld -Wl,--start-group $(LIBS) -Wl,--end-group

OBJS := \
	$(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/%.s.o,$(SRC_S)) \
	$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.c.o,$(SRC_C)) \
	$(BUILD_DIR)/Resources.o

DEPS := $(OBJS:.o=.d)

.PHONY: all clean dump run $(RMAKE)

all: $(ROM)

$(BUILD_DIR)/%.s.o: $(SRC_DIR)/%.s
	@echo "  AS      $<"
	@mkdir -p $(@D)
	@$(CC) $(ASFLAGS) -MMD -MP -c -o $@ $<

$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	@echo "  CC      $<"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILD_DIR)/Resources: $(RMAKE) $(RES_MANIFEST) $(RES_R)
	@echo "  RMAKE   $(RES_MANIFEST)"
	@mkdir -p $(@D)
	@$(RMAKE) $(RES_MANIFEST) > $@

$(BUILD_DIR)/Resources.o: $(BUILD_DIR)/Resources
	@echo "  OBJCOPY $<"
	@mkdir -p $(@D)
	@$(OBJCOPY) -I binary -O elf32-littlearm --rename-section .data=.res $< $@
	@$(OBJCOPY) --redefine-sym _binary_$(BUILD_DIR)_Resources_start=__Resources__ $@ $@

$(ROMFIX):
	@make -C tools/romfix

$(RMAKE):
	@make -C tools/rmake rmake

$(ELF): $(OBJS)
	@echo "  LD      $@"
	@mkdir -p $(@D)
	@$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(ROM): $(ELF) $(ROMFIX)
	@echo "  OBJCOPY $<"
	@mkdir -p $(@D)
	@$(OBJCOPY) -O binary $< $@
	@echo "  ROMFIX  $@"
	@$(ROMFIX) $@ -t $(GAME_TITLE) -c $(GAME_CODE)

$(DUMP): $(ELF)
	@echo "  OBJDUMP $@"
	@$(OBJDUMP) -h -C -S $< > $@

dump: $(DUMP)

clean:
	@echo "  CLEAN   $(BUILD_DIR) $(DIST_DIR)"
	@rm -rf $(BUILD_DIR) $(DIST_DIR)
	@make -C tools/romfix clean
	@make -C tools/rmake clean

run: $(ROM)
	@echo "  RUN     $<"
	@open $(ROM)
