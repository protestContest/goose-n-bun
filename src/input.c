#include "input.h"
#include "hardware.h"
#include "bios.h"
#include "interrupt.h"
#include "debug.h"

typedef struct {
  FnPtr onDown;
  FnPtr onUp;
} KeyHandler;

static KeyHandler handlers[10] = {0};

static void Wake(void)
{
  DisableInterrupts(INT_KEYPAD);
  REG_KEYCNT = 0;
}

void SleepUntil(u16 keys)
{
  REG_KEYCNT = keys | 0xC000;
  OnInterrupt(INT_KEYPAD, Wake);
  Stop();
}

void OnKeyDown(u16 keys, FnPtr handler)
{
  for (u32 i = 0; i < ArrayCount(handlers); i++) {
    if (keys & (1 << i)) {
      handlers[i].onDown = handler;
    }
  }
}

void OnKeyUp(u16 keys, FnPtr handler)
{
  for (u32 i = 0; i < ArrayCount(handlers); i++) {
    if (keys & (1 << i)) {
      handlers[i].onUp = handler;
    }
  }
}

void GetInput(void)
{
  static u16 lastKeys = 0;
  u16 keys = REG_KEYINPUT;

  for (u16 i = 0; i < ArrayCount(handlers); i++) {
    u16 key = 1 << i;
    if ((lastKeys & key) && !(keys & key) && handlers[i].onDown) {
      handlers[i].onDown();
    }
    if (!(lastKeys & key) && (keys & key) && handlers[i].onUp) {
      handlers[i].onUp();
    }
  }

  lastKeys = keys;
}
