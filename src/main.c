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

  AnimatedSprite sprites[4];

  enum {idleState, flapState, runState, walkState};

  InitSprite(&sprites[idleState], Obj32x32, 512, 2, 20);
  InitSprite(&sprites[flapState], Obj32x32, 512+16*2, 4, 10);
  InitSprite(&sprites[runState], Obj32x32, 512+16*6, 4, 4);
  InitSprite(&sprites[walkState], Obj32x32, 512+16*10, 4, 8);

  u32 state = idleState;
  Rect hitbox = {10, 10, 22, 32};
  i16 x = SCREEN_W/2 - (hitbox.right-hitbox.left)/2;
  i16 y = SCREEN_H/2 - (hitbox.bottom-hitbox.top)/2;

  AssignSprite(0, &sprites[state]);
  PlaceObj(0, x - hitbox.left, y - hitbox.top);

  while (true) {
    VSync();

    bool changed = UpdateSprite(0);

    u16 input = GetInput();

    if (input & BTN_DPAD) {
      u32 targetState = (input & BTN_B) ? runState : walkState;

      if (changed || state != targetState) {
        i16 speed = (state == walkState) ? 3 : 5;
        bool moved = false;

        if (input & BTN_LEFT) {
          speed = Min(x, speed);
          if (speed > 0) {
            x -= speed;
            moved = true;
          }
        } else if (input & BTN_RIGHT) {
          speed = Min(SCREEN_W-x-(hitbox.right - hitbox.left), speed);
          if (speed > 0) {
            x += speed;
            moved = true;
          }
        }
        speed = (state == walkState) ? 3 : 5;
        if (input & BTN_UP) {
          speed = Min(y, speed);
          if (speed > 0) {
            y -= speed;
            moved = true;
          }
        } else if (input & BTN_DOWN) {
          speed = Min(SCREEN_H-y-(hitbox.bottom - hitbox.top), speed);
          if (speed > 0) {
            y += speed;
            moved = true;
          }
        }
        if (!moved) {
          targetState = flapState;
        }
      PlaceObj(0, x - hitbox.left, y - hitbox.top);
      }

      if (state != targetState) {
        state = targetState;
        AssignSprite(0, &sprites[state]);
      }

      if (input & BTN_LEFT) {
        SetObjFlip(0, ObjFlipH);
      } else if (input & BTN_RIGHT) {
        SetObjFlip(0, 0);
      }
    } else {
      u32 targetState = (input & BTN_A) ? flapState : idleState;
      if (state != targetState) {
        state = targetState;
        AssignSprite(0, &sprites[state]);
      }
    }
  }
}
