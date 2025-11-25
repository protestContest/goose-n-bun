CC := gcc
CFLAGS := -Wall -O3

.PHONY: all clean

all: rmake res-extract

rmake: rmake.c
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

res-extract: res_extract.c
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "  CLEAN"
	@rm -rf rmake res-extract
