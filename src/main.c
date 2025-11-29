#include "canvas.h"
#include "text.h"
#include "str.h"
#include "input.h"
#include "interrupt.h"
#include "res.h"
#include "mem.h"
#include "debug.h"

void TestScreen(void)
{
  FontInfo font;
  SetColor(WHITE);
  char *fontName = "Geneva";
  SetFont(fontName);
  GetFontInfo(&font);

  char infoLine[37] = {0};
  Copy("Ascent: ^  Descent: ^  Leading: ^", infoLine, 33);
  FormatInt(infoLine, font.ascent, 36);
  FormatInt(infoLine, font.descent, 36);
  FormatInt(infoLine, font.leading, 36);

  char *lines[] = {
    fontName,
    "",
    "Jackdaws love my big sphinx of quartz",
    "",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    "abcdefghijklmnopqrstuvwxyz",
    "1234567890&@.,?!'\"()",
    "",
    infoLine
  };

  u32 height = ArrayCount(lines)*font.lineHeight;
  u32 start = SCREEN_H/2 - height/2 + font.ascent;
  for (u32 i = 0; i < ArrayCount(lines); i++) {
    u32 width = StringWidth(lines[i]);
    MoveTo(SCREEN_W/2 - width/2, start + i*font.lineHeight);
    Print(lines[i]);
  }
}

volatile bool foo = false;

void Foo(void)
{
  if (foo) {
    ClearScreen(BLUE);
  } else {
    ClearScreen(RED);
  }
  foo = !foo;
}

void main(void)
{
  EnableDebug();
  GraphicsMode(3);
  ShowLayer(DISP_BG2);
  SetColor(WHITE);
  SetFont("Geneva");

  MoveTo(100, 50);
  Print("Test");

  char *text = Uncompress(GetResource("test"), 0);
  if (!text) Error("Could not get text");

  MoveTo(100, 100);
  Print(text);

  OnKeyDown(BTN_A, Foo);

  while (true) {
    // VSync();
    // GetInput();
  }
}
