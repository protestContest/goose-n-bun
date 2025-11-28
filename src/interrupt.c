#include "interrupt.h"
#include "hardware.h"

static FnPtr handlers[14] = {0};

__attribute__((target("arm")))
static void HandleInterrupt(void)
{
  u16 fired = REG_IF;
  REG_IF = fired;
  REG_BIOSIF = fired;

  for (u16 i = 0; i < ArrayCount(handlers); i++) {
    if ((fired & (1 << i)) && handlers[i]) {
      handlers[i]();
    }
  }
}

void OnInterrupt(u16 interrupts, FnPtr handler)
{
  EnableInterrupts(false);
  ISR = HandleInterrupt;
  REG_IE |= interrupts;

  for (u16 i = 0; i < ArrayCount(handlers); i++) {
    if (interrupts & (1 << i)) {
      if ((1 << i) == INT_VBLANK) REG_DISPSTAT |= LCD_VBLANK_IRQ;
      if ((1 << i) == INT_HBLANK) REG_DISPSTAT |= LCD_HBLANK_IRQ;
      if ((1 << i) == INT_VCOUNT) REG_DISPSTAT |= LCD_VCOUNT_IRQ;
      handlers[i] = handler;
    }
  }

  EnableInterrupts(true);
}

void DisableInterrupts(u16 interrupts)
{
  EnableInterrupts(false);
  ISR = HandleInterrupt;
  REG_IE &= ~interrupts;

  for (u16 i = 0; i < ArrayCount(handlers); i++) {
    if (interrupts & (1 << i)) {
      if ((1 << i) == INT_VBLANK) REG_DISPSTAT &= ~LCD_VBLANK_IRQ;
      if ((1 << i) == INT_HBLANK) REG_DISPSTAT &= ~LCD_HBLANK_IRQ;
      if ((1 << i) == INT_VCOUNT) REG_DISPSTAT &= ~LCD_VCOUNT_IRQ;
      handlers[i] = 0;
    }
  }

  EnableInterrupts(true);
}

void VSync(void)
{
  EnableInterrupts(false);
  ISR = HandleInterrupt;
  REG_DISPSTAT |= LCD_VBLANK_IRQ;
  REG_IE |= INT_VBLANK;
  EnableInterrupts(true);
  VBlankIntrWait();
}
