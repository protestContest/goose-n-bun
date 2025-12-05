#include "interrupt.h"
#include "time.h"
#include "canvas.h"
#include "res.h"
#include "sprite.h"
#include "input.h"
#include "math.h"
#include "goose.h"
#include "bun.h"
#include "ui.h"
#include "text.h"
#include "str.h"

Character buns[126];
Character goose;
Rect holeRect;

bool Round(u32 numBuns)
{
  holeRect.left = RandomBetween(0, SCREEN_W - 18);
  holeRect.top = RandomBetween(0, SCREEN_H - 10);
  holeRect.right = holeRect.left + 18;
  holeRect.bottom = holeRect.top + 10;
  PlaceObj(127, holeRect.left - 7, holeRect.top - 4);

  for (u32 i = 0; i < numBuns; i++) {
    InitBun(&buns[i], i+1);
  }
  for (u32 i = numBuns; i < ArrayCount(buns); i++) {
    buns[i].state = -1;
  }

  bool paused = false;

  while (numBuns > 0) {
    VSync();

    u16 input = GetInput();
    if (KeyPressed(BTN_START)) {
      paused = !paused;
    }

    if (paused) continue;

    UpdateCharacter(&goose, input);

    for (u32 i = 0; i < ArrayCount(buns); i++) {
      if (buns[i].state < 0) continue;

      UpdateCharacter(&buns[i], BunAI(&buns[i], &goose));

      if (PtInRect(&holeRect, buns[i].pos)) {
        DestroyBun(&buns[i]);
        numBuns--;
      }

      if (buns[i].state == bunIdle && RandomBetween(0, 1000) < 1) {
        bool lost = true;
        for (u32 j = 0; j < ArrayCount(buns); j++) {
          if (buns[j].state < 0) {
            InitBun(&buns[j], j+1);
            PlaceCharacter(&buns[j], buns[i].pos.h, buns[i].pos.v);
            numBuns++;
            lost = false;
            break;
          }
        }
        if (lost) {
          for (u32 j = 0; j < ArrayCount(buns); j++) {
            if (buns[j].state >= 0) {
              DestroyBun(&buns[j]);
            }
          }
          return false;
        }
      }
    }
  }

  return true;
}

void Game(void)
{
  InitGoose(&goose, 0);
  ShowLayer(DISP_OBJ);

  u32 levels[] = {1, 3, 5, 8, 13};
  u32 curLevel = 0;

  while (true) {
    if (!Round(levels[curLevel])) {
      HideLayer(DISP_OBJ);
      Alert("You lose ");
      WaitForInput();
      break;
    }
    curLevel = Min(curLevel+1, ArrayCount(levels)+1);
  }

  HideLayer(DISP_OBJ);
}

void main(void)
{
  // EnableDebug();
  OnInterrupt(INT_VBLANK, UpdateTime);

  GraphicsMode(3);
  ShowLayer(DISP_BG2);
  ObjMapMode(ObjMap1D);

  SetFont("Geneva");

  TGA *grass = ResData(GetResource("grass.tga"));
  ShowImage(grass, 0, 0);

  TGA *tiles = ResData(GetResource("sprites.tga"));
  u8 *colors = (u8*)tiles->data;
  SetTiles(tiles);
  SetPalette(0, colors, tiles->paletteSize, tiles->paletteDepth);

  AnimatedSprite hole;
  InitSprite(&hole, Obj32x16, 776, 1, 0);
  AssignSprite(127, &hole);

  while (true) {
    SetFont("Venice");
    i16 textWidth = TextWidth("Goose & Bun ");
    MoveTo(SCREEN_W/2-textWidth/2, 40);
    SetColor(BLACK);
    Print("Goose & Bun ");
    MoveTo(SCREEN_W/2-textWidth/2-2, 38);
    SetColor(WHITE);
    Print("Goose & Bun ");
    SetFont("Geneva");
    Alert("Press Start ");

    while (true) {
      VSync();
      GetInput();
      if (KeyPressed(BTN_START)) break;
    }

    SeedRandom(TickCount());
    ShowImage(grass, 0, 0);
    Game();
  }
}
