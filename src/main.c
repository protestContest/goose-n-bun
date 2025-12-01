#include "canvas.h"
#include "text.h"
#include "str.h"
#include "input.h"
#include "interrupt.h"
#include "res.h"
#include "mem.h"
#include "debug.h"
#include "ui.h"
#include "math.h"
#include "time.h"

void TestScreen(void)
{
  FontInfo font;
  SetColor(WHITE);
  char *fontName = "Geneva";
  SetFont(fontName);
  GetFontInfo(&font);

  char text[256] = {0};
  char *t = text;
  t = StrCat(t, fontName);
  t = StrCat(t, "\n\n");
  t = StrCat(t, "Jackdaws love my big sphinx of quartz\n");
  t = StrCat(t, "\n");
  t = StrCat(t, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
  t = StrCat(t, "abcdefghijklmnopqrstuvwxyz\n");
  t = StrCat(t, "1234567890&@.,?!'\"()\n");
  t = StrCat(t, "\n");

  char infoLine[37] = {0};
  Copy("Ascent: ^  Descent: ^  Leading: ^", infoLine, 33);
  FormatInt(infoLine, font.ascent, 36);
  FormatInt(infoLine, font.descent, 36);
  FormatInt(infoLine, font.leading, 36);
  StrCat(t, infoLine);

  Alert(text);
}

char *haikus[] = {
  "The wren\nEarns his living\nNoiselessly.",
  "From time to time\nThe clouds give rest\nTo the moon-beholders.",
  "Over-ripe sushi,\nThe Master\nIs full of regret.",
  "Consider me\nAs one who loved poetry\nAnd persimmons.",
  "In the cicada's cry\nNo sign can foretell\nHow soon it must die.",
  "Blowing from the west\nFallen leaves gather\nIn the east.",
  "Winter seclusion -\nListening, that evening,\nTo the rain in the mountain.",
  "Don't weep, insects -\nLovers, stars themselves,\nMust part.",
  "My life, -\nHow much more of it remains?\nThe night is brief.",
  "An old silent pond...\nA frog jumps into the pond,\nsplash! Silence again.",
  "I kill an ant\nand realize my three children\nhave been watching.",
  "Over the wintry\nforest, winds howl in rage\nwith no leaves to blow.",
};

void Popup(void)
{
  RandBetween(0, 12);
  char *text = haikus[RandBetween(0, ArrayCount(haikus))];
  i16 x = RandBetween(0, SCREEN_W - TextWidth(text) - 16);
  i16 y = RandBetween(0, SCREEN_H - TextHeight(text) - 16);
  TextWindow(text, x, y);
  SetTimeout(20, Popup);
}

void main(void)
{
  EnableDebug();

  OnInterrupt(INT_VBLANK, UpdateTime);

  GraphicsMode(3);
  ShowLayer(DISP_BG2);
  ClearScreen(RGB(240, 192, 136));
  SetFont("Geneva");

  // Rect r = {100, 50, 150, 100};
  // // PenSize(3, 3);
  // FrameRoundRect(&r, 8, 8);

  Rect r = {100, 50, 140, 90};
  FillArc(&r, 0, 360, BLACK);


  // TestScreen();

  // OnKeyDown(BTN_A, Popup);

  while (true) {
    // VSync();
    // CheckTimeouts();
    // GetInput();
  }
}
