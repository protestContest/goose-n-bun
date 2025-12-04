#include "canvas.h"
#include "text.h"
#include "input.h"
#include "interrupt.h"
#include "res.h"
#include "debug.h"
#include "time.h"
#include "sprite.h"

typedef struct {
  u16 obj;
  i16 x;
  i16 y;
  Rect box;
  u16 state;
  u16 numStates;
  AnimatedSprite *sprites;
} Character;

void PlaceCharacter(Character *ch, i16 x, i16 y)
{
  ch->x = x;
  ch->y = y;
  PlaceObj(ch->obj, ch->x - ch->box.left, ch->y - ch->box.top);
}

void SetCharacterState(Character *ch, u16 targetState)
{
  if (ch->state != targetState) {
    ch->state = targetState;
    AssignSprite(ch->obj, &ch->sprites[ch->state]);
  }
}

enum {bunIdle, bunHop, bunHopVert};

void UpdateBun(Character *ch, u32 input)
{
  bool changed = UpdateSprite(ch->obj);

  if (input & BTN_DPAD) {
    u32 targetState = (input & (BTN_LEFT | BTN_RIGHT)) ? bunHop : bunHopVert;

    if (changed || ch->state != targetState) {
      bool moved = false;

      i16 speed = 4;
      if (input & BTN_LEFT) {
        speed = Min(ch->x, speed);
        if (speed > 0) {
          ch->x -= speed;
          moved = true;
        }
      } else if (input & BTN_RIGHT) {
        speed = Min(SCREEN_W-ch->x-(ch->box.right - ch->box.left), speed);
        if (speed > 0) {
          ch->x += speed;
          moved = true;
        }
      }

      speed = 3;
      if (input & BTN_UP) {
        speed = Min(ch->y, speed);
        if (speed > 0) {
          ch->y -= speed;
          moved = true;
        }
      } else if (input & BTN_DOWN) {
        speed = Min(SCREEN_H-ch->y-(ch->box.bottom - ch->box.top), speed);
        if (speed > 0) {
          ch->y += speed;
          moved = true;
        }
      }

      if (!moved) {
        targetState = bunIdle;
      }

      PlaceCharacter(ch, ch->x, ch->y);
    }

    SetCharacterState(ch, targetState);

    if (input & BTN_DOWN) {
      SetObjFlipV(ch->obj, true);
    } else if (input & BTN_UP) {
      SetObjFlipV(ch->obj, false);
    }
    if (input & BTN_RIGHT) {
      SetObjFlipH(ch->obj, true);
      SetObjFlipV(ch->obj, false);
    } else if (input & BTN_LEFT) {
      SetObjFlipH(ch->obj, false);
      SetObjFlipV(ch->obj, false);
    }
  } else {
    SetObjFlipV(ch->obj, false);
    SetCharacterState(ch, bunIdle);
  }
}

enum {gooseIdle, gooseFlap, gooseRun, gooseWalk};

void UpdateGoose(Character *ch, u32 input)
{
  bool changed = UpdateSprite(ch->obj);

  if (input & BTN_DPAD) {
    u32 targetState = (input & BTN_B) ? gooseRun : gooseWalk;

    if (changed || ch->state != targetState) {
      bool moved = false;

      i16 speed = (ch->state == gooseWalk) ? 3 : 5;
      if (input & BTN_LEFT) {
        speed = Min(ch->x, speed);
        if (speed > 0) {
          ch->x -= speed;
          moved = true;
        }
      } else if (input & BTN_RIGHT) {
        speed = Min(SCREEN_W-ch->x-(ch->box.right - ch->box.left), speed);
        if (speed > 0) {
          ch->x += speed;
          moved = true;
        }
      }

      speed = (ch->state == gooseWalk) ? 2 : 3;
      if (input & BTN_UP) {
        speed = Min(ch->y, speed);
        if (speed > 0) {
          ch->y -= speed;
          moved = true;
        }
      } else if (input & BTN_DOWN) {
        speed = Min(SCREEN_H-ch->y-(ch->box.bottom - ch->box.top), speed);
        if (speed > 0) {
          ch->y += speed;
          moved = true;
        }
      }

      if (!moved) {
        targetState = gooseFlap;
      }

      PlaceCharacter(ch, ch->x, ch->y);
    }

    SetCharacterState(ch, targetState);

    if (input & BTN_LEFT) {
      SetObjFlipH(ch->obj, true);
    } else if (input & BTN_RIGHT) {
      SetObjFlipH(ch->obj, false);
    }
  } else {
    u32 targetState = (input & BTN_A) ? gooseFlap : gooseIdle;
    SetCharacterState(ch, targetState);
  }
}

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

  AnimatedSprite gooseSprites[4];
  InitSprite(&gooseSprites[gooseIdle], Obj32x32, 512, 2, 20);
  InitSprite(&gooseSprites[gooseFlap], Obj32x32, 512+16*2, 4, 10);
  InitSprite(&gooseSprites[gooseRun], Obj32x32, 512+16*6, 4, 4);
  InitSprite(&gooseSprites[gooseWalk], Obj32x32, 512+16*10, 4, 8);

  Character goose;
  goose.obj = 0;
  goose.box.left = 10;
  goose.box.right = 10;
  goose.box.top = 22;
  goose.box.bottom = 32;
  goose.x = SCREEN_W/2 - (goose.box.right-goose.box.left)/2;
  goose.y = SCREEN_H/2 - (goose.box.bottom-goose.box.top)/2;
  goose.state = gooseIdle;
  goose.numStates = 4;
  goose.sprites = gooseSprites;

  AssignSprite(goose.obj, &goose.sprites[goose.state]);
  PlaceCharacter(&goose, goose.x, goose.y);

  AnimatedSprite bunSprites[3];
  InitSprite(&bunSprites[bunIdle], Obj16x16, 736, 2, 18);
  InitSprite(&bunSprites[bunHop], Obj16x16, 744, 4, 8);
  InitSprite(&bunSprites[bunHopVert], Obj16x16, 760, 4, 8);

  Character bun;
  bun.obj = 1;
  bun.box.left = 0;
  bun.box.top = 0;
  bun.box.right = 16;
  bun.box.bottom = 16;
  bun.x = 160;
  bun.y = 70;
  bun.state = 0;
  bun.numStates = 1;
  bun.sprites = bunSprites;
  AssignSprite(bun.obj, &bun.sprites[0]);
  PlaceCharacter(&bun, bun.x, bun.y);

  bool isGoose = true;
  u16 lastInput = GetInput();

  while (true) {
    VSync();

    u16 input = GetInput();
    if (KeyPressed(lastInput, input, BTN_SELECT)) {
      isGoose = !isGoose;
    }

    if (isGoose) {
      UpdateGoose(&goose, input);
      UpdateBun(&bun, 0);
    } else {
      UpdateGoose(&goose, 0);
      UpdateBun(&bun, input);
    }

    lastInput = input;
  }
}
