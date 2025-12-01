#include "time.h"
#include "hardware.h"
#include "interrupt.h"
#include "mem.h"

static u32 ticks = 0;

typedef struct {
  u32 time;
  FnPtr fn;
} Timeout;

static Timeout timeouts[256] = {0};
static u32 numTimeouts = 0;

void UpdateTime(void)
{
  ticks++;
}

void CheckTimeouts(void)
{
  if (numTimeouts && timeouts[numTimeouts-1].time <= ticks) {
    timeouts[--numTimeouts].fn();
  }
}

void SetTimeout(u32 dt, FnPtr fn)
{
  u32 time = ticks + dt;
  if (numTimeouts == 0) {
    timeouts[numTimeouts].time = time;
    timeouts[numTimeouts].fn = fn;
    numTimeouts++;
    return;
  }

  u32 i = 0;
  while (i < numTimeouts && time < timeouts[i].time) {
    i++;
  }

  for (u32 j = i; j < numTimeouts; j++) {
    timeouts[j+1] = timeouts[j];
  }

  timeouts[i].time = time;
  timeouts[i].fn = fn;
  numTimeouts++;
}

u32 TickCount(void)
{
  return ticks;
}

void Delay(u32 ms)
{
  OnInterrupt(INT_TIMER3, 0);
  u32 ticks = (ms * 16386) / 1000;
  REG_TM3CNT_L = 0xFFFF - ticks;
  REG_TM3CNT_H = 0xC3;
  IntrWait(true, INT_TIMER3);
}
