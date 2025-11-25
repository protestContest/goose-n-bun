OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(entrypoint)

MEMORY
{
  ROM   : ORIGIN = 0x08000000, LENGTH = 32M
  IWRAM : ORIGIN = 0x03000000, LENGTH = 32K
  EWRAM : ORIGIN = 0x02000000, LENGTH = 256K
}

__STACK_SVC_SIZE__      = 0x40;
__STACK_IRQ_SIZE__      = 0xA0;
__STACK_USR_SIZE_MIN__  = 0x200; /* Minimum size, the real size may be bigger */

__STACK_END__       = ORIGIN(IWRAM) + LENGTH(IWRAM) - 0x20; /* Used by BIOS */
__STACK_SVC_END__   = __STACK_END__;
__STACK_SVC_START__ = __STACK_SVC_END__ - __STACK_SVC_SIZE__;
__STACK_IRQ_END__   = __STACK_SVC_START__;
__STACK_IRQ_START__ = __STACK_IRQ_END__ - __STACK_IRQ_SIZE__;
__STACK_USR_END__   = __STACK_IRQ_START__;
__STACK_USR_START__ = __STACK_USR_END__ - __STACK_USR_SIZE_MIN__;
__STACK_START__     = __STACK_USR_START__;

SECTIONS
{
  .gba_crt0 : ALIGN(4)
  {
      KEEP (*(.gba_crt0))
  } > ROM

  .text : ALIGN(4)
  {
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    KEEP(*(.init))
    KEEP(*(.fini))
  } > ROM

  .rodata : ALIGN(4)
  {
    *(.rodata)
    *(.rodata*)
  } > ROM

  .res (READONLY) : ALIGN(4)
  {
    KEEP (*(.res))
  } > ROM

  .bss (NOLOAD) : ALIGN(4)
  {
      *(.bss)
      *(.bss*)
      *(COMMON)
      . = ALIGN(4);
  } > IWRAM

  __IWRAM_BSS_START__ = ADDR(.bss);
  __IWRAM_BSS_SIZE__ = SIZEOF(.bss);
  __IWRAM_BSS_END__ = __IWRAM_BSS_START__ + __IWRAM_BSS_SIZE__;

  .data : ALIGN(4)
  {
    __DATA_START__ = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    __DATA_END__ = .;
  } > IWRAM AT> ROM

  __DATA_SIZE__ = __DATA_END__ - __DATA_START__;
  __DATA_LMA__ = LOADADDR(.data);

  .iwram : ALIGN(4)
  {
    __IWRAM_START__ = .;
    *(.iwram)
    *(.iwram*)
    *iwram.*(.text*)
    *iwram.*(.data*)
    . = ALIGN(4);
    __IWRAM_END__ = .;
  } > IWRAM AT> ROM

  __IWRAM_SIZE__ = __IWRAM_END__ - __IWRAM_START__;
  __IWRAM_LMA__ = LOADADDR(.iwram);

  ASSERT(__IWRAM_END__ <= __STACK_START__, "Not enough free IWRAM for stack")

  __STACK_USR_SIZE__ = __STACK_USR_END__ - __IWRAM_END__;

  .sbss (NOLOAD) : ALIGN(4)
  {
    *(.sbss)
    *(.sbss*)
    . = ALIGN(4);
  } > EWRAM

  .ewram : ALIGN(4)
  {
    __EWRAM_START__ = .;
    *(.ewram)
    *(.ewram*)
    *ewram.*(.text*)
    *ewram.*(.data*)
    . = ALIGN(4);
    __EWRAM_END__ = .;
  } > EWRAM AT > ROM

  __EWRAM_SIZE__ = __EWRAM_END__ - __EWRAM_START__;
  __EWRAM_LMA__ = LOADADDR(.ewram);

  __HEAP_START__  = __EWRAM_END__;
  __HEAP_END__    = ORIGIN(EWRAM) + LENGTH(EWRAM);
  __HEAP_SIZE__   = __HEAP_END__ - __HEAP_START__;
}


