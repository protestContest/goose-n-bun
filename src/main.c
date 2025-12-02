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
  // SetTimeout(20, Popup);
}

typedef struct __attribute__((packed)) {
  u8 header[8];
  u16 x;
  u16 y;
  u16 width;
  u16 height;
  u8 depth;
  u8 description;
  u16 data[];
} TGA;

void ShowImage(TGA *image)
{
  // TGA *image = ResData(GetResource(name));
  if (!image) return;

  for (u32 i = 0; i < image->height; i++) {
    for (u32 j = 0; j < image->width; j++) {
      u16 pixel = image->data[i * image->width + j];
      if (!(pixel & 0x8000)) continue;

      WritePixel(image->x + j, image->y - image->height + i, pixel);
    }
  }
}

typedef struct {
  u32 count;
  u32 speed;
  bool repeat;
  u32 curFrame;
  u32 nextAt;
  TGA **frames;
} Sprite;

void PlaySprite(Sprite *sprite)
{
  u32 now = TickCount();
  if (sprite->curFrame < sprite->count && sprite->nextAt <= now) {
    sprite->nextAt = now + sprite->speed;

    u32 lastIndex = sprite->curFrame == 0 ? sprite->count-1 : sprite->curFrame-1;
    TGA *last = sprite->frames[lastIndex];
    Rect r = {
      last->x,
      last->y - last->height,
      last->x + last->width,
      last->y
    };
    FillRect(&r, RGB(240, 192, 136));

    ShowImage(sprite->frames[sprite->curFrame]);
    sprite->curFrame++;
    if (sprite->curFrame == sprite->count && sprite->repeat) sprite->curFrame = 0;
  }
}

Sprite sprite;

void Restart(void)
{
  sprite.curFrame = 0;
}

void main(void)
{
  EnableDebug();
  OnInterrupt(INT_VBLANK, UpdateTime);

  GraphicsMode(3);
  ShowLayer(DISP_BG2);
  SetFont("Geneva");

  ClearScreen(RGB(240, 192, 136));

  OnKeyDown(BTN_A, Restart);

  TGA *frames[] = {
    ResData(GetResource("dice/1.tga")),
    ResData(GetResource("dice/2.tga")),
    ResData(GetResource("dice/3.tga")),
    ResData(GetResource("dice/4.tga")),
    ResData(GetResource("dice/5.tga")),
    ResData(GetResource("dice/6.tga")),
    ResData(GetResource("dice/7.tga")),
    ResData(GetResource("dice/8.tga")),
    ResData(GetResource("dice/9.tga")),
    ResData(GetResource("dice/10.tga")),
    ResData(GetResource("dice/11.tga")),
    ResData(GetResource("dice/12.tga")),
    ResData(GetResource("dice/13.tga")),
    ResData(GetResource("dice/14.tga")),
    ResData(GetResource("dice/15.tga")),
    ResData(GetResource("dice/16.tga")),
    ResData(GetResource("dice/17.tga")),
    ResData(GetResource("dice/18.tga")),
    ResData(GetResource("dice/19.tga")),
    ResData(GetResource("dice/20.tga")),
  };

  sprite.count = ArrayCount(frames);
  sprite.speed = 6;
  sprite.repeat = false;
  sprite.curFrame = 0;
  sprite.nextAt = 0;
  sprite.frames = frames;

  while (true) {
    VSync();
    PlaySprite(&sprite);
    // CheckTimeouts();
    GetInput();
  }
}
