CC := gcc
CFLAGS := -Wall -O3

.PHONY: all clean

all: romfix

romfix: romfix.c
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

res-extract: res_extract.c
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "  CLEAN   romfix"
	@rm -rf romfix
