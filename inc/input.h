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

void SleepUntil(u16 keys);
void OnKeyDown(u16 keys, FnPtr handler);
void OnKeyUp(u16 keys, FnPtr handler);
void GetInput(void);
