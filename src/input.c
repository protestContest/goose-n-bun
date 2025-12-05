#include "input.h"
#include "hardware.h"
#include "interrupt.h"
#include "debug.h"

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

void WaitForInput(void)
{
  IntrWait(true, INT_KEYPAD);
}

static u16 keys = 0;
static u16 lastKeys = 0;

u16 GetInput(void)
{
  lastKeys = keys;
  keys = ~REG_KEYINPUT;
  return keys;
}

bool KeyPressed(u16 btn)
{
  return (lastKeys ^ keys) & keys & btn;
}

bool KeyReleased(u16 btn)
{
  return (lastKeys ^ keys) & lastKeys & btn;
}
