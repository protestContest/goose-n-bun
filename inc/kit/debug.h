#pragma once

#define Assert(cond)  (void)((cond) || (Error("Assertion failed"),0))

bool EnableDebug(void);
void Log(char *msg);
void Error(char *msg);
