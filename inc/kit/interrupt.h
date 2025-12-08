#pragma once
#include "hardware.h"

#define EnableInterrupts(enable) do {\
  REG_IME = (enable);\
} while (0)

void OnInterrupt(u16 interrupts, FnPtr handler);
void DisableInterrupts(u16 interrupts);
void VSync(void);
