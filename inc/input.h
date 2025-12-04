#pragma once

#define BTN_A       (1 << 0)
#define BTN_B       (1 << 1)
#define BTN_SELECT  (1 << 2)
#define BTN_START   (1 << 3)
#define BTN_RIGHT   (1 << 4)
#define BTN_LEFT    (1 << 5)
#define BTN_UP      (1 << 6)
#define BTN_DOWN    (1 << 7)
#define BTN_R       (1 << 8)
#define BTN_L       (1 << 9)
#define BTN_DPAD    (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT)

void SleepUntil(u16 keys);
void WaitForInput(void);
u16 GetInput(void);
