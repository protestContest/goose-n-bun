#include "debug.h"
#include "canvas.h"
#include "text.h"

void Error(char *msg)
{
  SetColor(RED);
  MoveTo(115, 75);
  Line(10, 10);
  MoveTo(115, 85);
  Line(10, -10);
  if (msg) {
    u32 width = StringWidth(msg);
    MoveTo(SCREEN_W/2 - width/2, 100);
    Print(msg);
  }

  while (1);
}
