#include "canvas.h"
#include "text.h"
#include "input.h"
#include "interrupt.h"
#include "res.h"
#include "debug.h"
#include "time.h"
#include "sprite.h"

void main(void)
{
  EnableDebug();
  OnInterrupt(INT_VBLANK, UpdateTime);

  GraphicsMode(3);
  ShowLayer(DISP_BG2);
  ShowLayer(DISP_OBJ);
  ObjMapMode(ObjMap1D);

  SetFont("Geneva");
  ClearScreen(GRAY);

  TGA *tga = ResData(GetResource("sprites.tga"));
  u8 *colors = (u8*)tga->data;
  u8 *pixels = ((u8*)tga->data) + tga->paletteSize*tga->paletteDepth/8;
  SetTiles(pixels, tga->width);
  SetPalette(0, colors, tga->paletteSize, tga->paletteDepth);

  AnimatedSprite idle, flap, run, walk;

  InitSprite(&idle, 0, Obj32x32, 512, 2, 15);
  PlaceObj(0, 40, 20);

  InitSprite(&flap, 1, Obj32x32, 512+16*2, 4, 5);
  PlaceObj(1, 160, 20);

  InitSprite(&run, 2, Obj32x32, 512+16*6, 4, 5);
  PlaceObj(2, 40, 100);

  InitSprite(&walk, 3, Obj32x32, 512+16*10, 4, 8);
  PlaceObj(3, 160, 100);

  while (true) {
    VSync();
    u32 now = TickCount();
    UpdateSprite(&idle, now);
    UpdateSprite(&flap, now);
    UpdateSprite(&run, now);
    UpdateSprite(&walk, now);

    GetInput();
  }
}
