#include "debug.h"
#include "canvas.h"
#include "text.h"
#include "mem.h"
#include "str.h"

#define MGBA_LOG_FATAL 0
#define MGBA_LOG_ERROR 1
#define MGBA_LOG_WARN 2
#define MGBA_LOG_INFO 3
#define MGBA_LOG_DEBUG 4

#define REG_DEBUG_ENABLE (volatile u16*) 0x4FFF780
#define REG_DEBUG_FLAGS (volatile u16*) 0x4FFF700
#define REG_DEBUG_STRING (char*) 0x4FFF600

bool EnableDebug(void)
{
  *REG_DEBUG_ENABLE = 0xC0DE;
  return *REG_DEBUG_ENABLE == 0x1DEA;
}

void Log(char *msg)
{
  u32 len = StrLen(msg);
  Copy(msg, REG_DEBUG_STRING, len+1);
  *REG_DEBUG_FLAGS = MGBA_LOG_INFO | 0x100;
}

void Error(char *msg)
{
  SetColor(RED);
  MoveTo(115, 75);
  Line(10, 10);
  MoveTo(115, 85);
  Line(10, -10);
  if (msg) {
    u32 width = TextWidth(msg);
    MoveTo(SCREEN_W/2 - width/2, 100);
    Print(msg);
  }

  while (1);
}
