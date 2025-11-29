#pragma once

#define Assert(cond)  ((cond) || (Error(0),0))

bool EnableDebug(void);
void Log(char *msg);
void Error(char *msg);
