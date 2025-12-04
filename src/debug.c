#include "debug.h"
#include "canvas.h"
#include "text.h"
#include "mem.h"
#include "str.h"
#include "res.h"

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
  ClearScreen(GRAY);
  HideLayer(DISP_OBJ);

  TGA *stop = Uncompress(GetResource("stop.tga"), 0);
  ShowImage(stop, SCREEN_W/2 - stop->width/2, SCREEN_H/2 - stop->height/2);

  if (msg) {
    FontInfo info;
    GetFontInfo(&info);
    u32 width = TextWidth(msg);
    u32 height = TextHeight(msg);
    SetColor(WHITE);
    MoveTo(SCREEN_W/2 - width/2, 3*SCREEN_H/4 - height/2 + info.ascent);
    Print(msg);
  }

  IntrWait(true, 0);
}
