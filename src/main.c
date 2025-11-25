#include "canvas.h"
#include "text.h"
#include "str.h"

int main(void)
{
  FontInfo font;

  GraphicsMode(3);
  ShowLayer(DISP_BG2);
  SetColor(WHITE);

  SetFont("Geneva-9");
  GetFontInfo(&font);

  char *lines[] = {
    "Sphinx of black quartz, judge my vow",
    "",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    "abcdefghijklmnopqrstuvwxyz",
    "1234567890&@.,?!'\"()",
    "",
    "Jackdaws love my big sphinx of quartz",
  };

  u32 height = ArrayCount(lines)*font.lineHeight;
  u32 start = SCREEN_H/2 - height/2 + font.ascent;
  for (u32 i = 0; i < ArrayCount(lines); i++) {
    u32 width = StringWidth(lines[i]);
    MoveTo(SCREEN_W/2 - width/2, start + i*font.lineHeight);
    Print(lines[i]);
  }

  while (1);

  return 0;
}
