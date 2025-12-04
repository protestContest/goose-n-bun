#include "canvas.h"
#include "text.h"
#include "input.h"
#include "interrupt.h"
#include "res.h"
#include "debug.h"
#include "time.h"
#include "sprite.h"
#include "math.h"
#include "mem.h"

struct Character;

typedef void (*CharacterUpdate)(struct Character *ch, u16 input);

typedef struct Character {
  u16 obj;
  i16 x;
  i16 y;
  Rect box;
  u16 state;
  u16 numStates;
  AnimatedSprite *sprites;
  u16 action;
  CharacterUpdate update;
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

void UpdateCharacter(Character *ch, u16 input)
{
  ch->update(ch, input);
}

enum {bunIdle, bunHop, bunHopVert};

void UpdateBun(Character *ch, u16 input)
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

void InitBun(Character *bun, u32 obj)
{
  bun->x = RandomBetween(10, SCREEN_W - 26);
  bun->y = RandomBetween(10, SCREEN_H - 26);
  bun->obj = obj;
  bun->box.left = 0;
  bun->box.top = 0;
  bun->box.right = 16;
  bun->box.bottom = 16;
  bun->state = 0;
  bun->numStates = 3;
  bun->sprites = Alloc(sizeof(AnimatedSprite)*3);
  InitSprite(&bun->sprites[bunIdle], Obj16x16, 736, 2, 18);
  InitSprite(&bun->sprites[bunHop], Obj16x16, 744, 4, 8);
  InitSprite(&bun->sprites[bunHopVert], Obj16x16, 760, 4, 8);

  bun->action = 0;
  bun->update = UpdateBun;
  AssignSprite(bun->obj, &bun->sprites[0]);
  PlaceCharacter(bun, bun->x, bun->y);
}

enum {gooseIdle, gooseFlap, gooseRun, gooseWalk};

void UpdateGoose(Character *ch, u16 input)
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

static AnimatedSprite gooseSprites[4];

void InitGoose(Character *goose, u32 obj)
{
  goose->x = RandomBetween(10, SCREEN_W - 26);
  goose->y = RandomBetween(10, SCREEN_H - 26);
  goose->obj = obj;
  goose->box.left = 0;
  goose->box.top = 0;
  goose->box.right = 16;
  goose->box.bottom = 16;
  goose->state = 0;
  goose->numStates = 1;
  goose->sprites = gooseSprites;
  goose->action = 0;
  goose->update = UpdateGoose;
  AssignSprite(goose->obj, &goose->sprites[0]);
  PlaceCharacter(goose, goose->x, goose->y);
}

u16 BunAI(Character *bun)
{
  if (bun->state == bunIdle) {
    if (RandomBetween(0, 1000) < 5) {
      switch (RandomBetween(0, 8)) {
      case 0:
        bun->action = BTN_LEFT;
        break;
      case 1:
        bun->action = BTN_RIGHT;
        break;
      case 2:
        bun->action = BTN_UP;
        break;
      case 3:
        bun->action = BTN_DOWN;
        break;
      case 4:
        bun->action = BTN_LEFT | BTN_UP;
        break;
      case 5:
        bun->action = BTN_LEFT | BTN_DOWN;
        break;
      case 6:
        bun->action = BTN_RIGHT | BTN_UP;
        break;
      case 7:
        bun->action = BTN_RIGHT | BTN_DOWN;
        break;
      }
    } else {
      bun->action = 0;
    }
  } else {
    if (RandomBetween(0, 100) < 5) bun->action = 0;
  }
  return bun->action;
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

  InitSprite(&gooseSprites[gooseIdle], Obj32x32, 512, 2, 20);
  InitSprite(&gooseSprites[gooseFlap], Obj32x32, 512+16*2, 4, 10);
  InitSprite(&gooseSprites[gooseRun], Obj32x32, 512+16*6, 4, 4);
  InitSprite(&gooseSprites[gooseWalk], Obj32x32, 512+16*10, 4, 8);

  Character chars[12];
  InitGoose(&chars[0], 0);
  for (u32 i = 1; i < ArrayCount(chars); i++) {
    InitBun(&chars[i], i);
  }

  u32 curChar = 0;

  u16 lastInput = GetInput();

  while (true) {
    VSync();

    u16 input = GetInput();
    if (KeyPressed(lastInput, input, BTN_L)) {
      curChar = (curChar == 0) ? ArrayCount(chars)-1 : curChar - 1;
    } else if (KeyPressed(lastInput, input, BTN_R)) {
      curChar = (curChar + 1) % ArrayCount(chars);
    }

    for (u32 i = 0; i < ArrayCount(chars); i++) {
      if (i == curChar) {
        UpdateCharacter(&chars[i], input);
      } else {
        UpdateCharacter(&chars[i], BunAI(&chars[i]));
      }
    }

    lastInput = input;
  }
}
