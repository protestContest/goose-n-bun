#pragma once

u32 TickCount(void);
void UpdateTime(void);
void CheckTimeouts(void);
void SetTimeout(u32 dt, FnPtr fn);
void Delay(u32 ms);
