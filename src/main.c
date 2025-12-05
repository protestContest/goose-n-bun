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
  Point spriteOffset;
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
  PlaceObj(ch->obj, ch->x + ch->spriteOffset.h, ch->y + ch->spriteOffset.v);
}

void SetCharacterState(Character *ch, u16 targetState)
{
  if (ch->state != targetState) {
    ch->state = targetState;
    AssignSprite(ch->obj, &ch->sprites[ch->state]);
  }
}

bool UpdateCharacterMovement(Character *ch, u16 input, i16 speed)
{
  bool changed = UpdateSprite(ch->obj);
  bool moved = false;

  if (input & BTN_DPAD) {
    i16 speedX = speed;
    if (input & BTN_LEFT) {
      speedX = Min(ch->x+ch->box.left, speedX);
      if (speedX > 0) {
        if (changed) {
          ch->x -= speedX;
        }
        moved = true;
      }
    } else if (input & BTN_RIGHT) {
      speedX = Min(SCREEN_W-ch->x-ch->box.right, speedX);
      if (speedX > 0) {
        if (changed) {
          ch->x += speedX;
        }
        moved = true;
      }
    }

    i16 speedY = speed;
    if (input & BTN_UP) {
      speedY = Min(ch->y+ch->box.top, speedY);
      if (speedY > 0) {
        if (changed) {
          ch->y -= speedY;
        }
        moved = true;
      }
    } else if (input & BTN_DOWN) {
      speedY = Min(SCREEN_H-ch->y-ch->box.bottom, speedY);
      if (speedY > 0) {
        if (changed) {
          ch->y += speedY;
        }
        moved = true;
      }
    }

    PlaceCharacter(ch, ch->x, ch->y);
  }

  return moved;
}

void UpdateCharacter(Character *ch, u16 input)
{
  ch->update(ch, input);
}

enum {bunIdle, bunHop, bunHopVert};

void UpdateBun(Character *ch, u16 input)
{
  bool moved = UpdateCharacterMovement(ch, input, 4);

  u32 targetState = bunIdle;
  if (moved) {
    if (input & (BTN_LEFT | BTN_RIGHT)) {
      targetState = bunHop;
    } else {
      targetState = bunHopVert;
    }
  }

  if (input & BTN_DOWN) {
    SetObjFlipV(ch->obj, true);
  } else if (input & BTN_UP) {
    SetObjFlipV(ch->obj, false);
  }
  if (input & BTN_RIGHT) {
    SetObjFlipH(ch->obj, true);
  } else if (input & BTN_LEFT) {
    SetObjFlipH(ch->obj, false);
  }
  if (ch->state != bunHopVert) {
    SetObjFlipV(ch->obj, false);
  }
  SetCharacterState(ch, targetState);
}

void InitBun(Character *bun, u32 obj)
{
  bun->x = RandomBetween(10, SCREEN_W - 26);
  bun->y = RandomBetween(10, SCREEN_H - 26);
  bun->obj = obj;
  bun->box.left = -5;
  bun->box.top = -5;
  bun->box.right = 5;
  bun->box.bottom = 0;
  bun->spriteOffset.h = -8;
  bun->spriteOffset.v = -16;
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
  i16 speed = (ch->state == gooseRun) ? 5 : 3;
  bool moved = UpdateCharacterMovement(ch, input, speed);

  if (moved) {
    if (input & BTN_B) {
      SetCharacterState(ch, gooseRun);
    } else {
      SetCharacterState(ch, gooseWalk);
    }
  } else if (input & BTN_A) {
    SetCharacterState(ch, gooseFlap);
  } else {
    SetCharacterState(ch, gooseIdle);
  }

  if (input & BTN_LEFT) {
    SetObjFlipH(ch->obj, true);
  } else if (input & BTN_RIGHT) {
    SetObjFlipH(ch->obj, false);
  }
}

static AnimatedSprite gooseSprites[4];

void InitGoose(Character *goose, u32 obj)
{
  goose->x = SCREEN_W/2;
  goose->y = SCREEN_H/2;
  goose->obj = obj;
  goose->box.left = -5;
  goose->box.top = -5;
  goose->box.right = 5;
  goose->box.bottom = 0;
  goose->spriteOffset.h = -16;
  goose->spriteOffset.v = -32;
  goose->state = 0;
  goose->numStates = 1;
  goose->sprites = gooseSprites;
  goose->action = 0;
  goose->update = UpdateGoose;
  AssignSprite(goose->obj, &goose->sprites[0]);
  PlaceCharacter(goose, goose->x, goose->y);
}

u16 BunAI(Character *bun, Character *goose)
{
  if (goose->state == gooseFlap || goose->state == gooseRun) {
    i16 tooClose = 30;
    i16 dx = goose->x - bun->x;
    i16 dy = goose->y - bun->y;
    if (Sqrt(dx*dx + dy*dy) < tooClose) {
      if (dy > 2*Abs(dx)) {
        bun->action = BTN_UP;
      } else if (dy < -2*Abs(dx)) {
        bun->action = BTN_DOWN;
      } else if (dx > 2*Abs(dy)) {
        bun->action = BTN_LEFT;
      } else if (dx < -2*Abs(dy)) {
        bun->action = BTN_RIGHT;
      } else if (dx > 0 && dy > 0) {
        bun->action = BTN_LEFT | BTN_UP;
      } else if (dx > 0 && dy < 0) {
        bun->action = BTN_LEFT | BTN_DOWN;
      } else if (dx < 0 && dy > 0) {
        bun->action = BTN_RIGHT | BTN_UP;
      } else if (dx < 0 && dy < 0) {
        bun->action = BTN_RIGHT | BTN_DOWN;
      }

      if (bun->x + bun->box.right >= SCREEN_W - 4 ||
          bun->x + bun->box.left <= 4) {
        if (dy > 0) {
          bun->action |= BTN_UP;
        } else {
          bun->action |= BTN_DOWN;
        }
      } else if (bun->y + bun->box.bottom >= SCREEN_H - 4 ||
          bun->y + bun->box.top <= 4) {
        if (dx > 0) {
          bun->action |= BTN_LEFT;
        } else {
          bun->action |= BTN_RIGHT;
        }
      }

      return bun->action;
    }
  }

  if (bun->state == bunIdle) {
    if (bun->x + bun->box.right == SCREEN_W) {
      bun->action = BTN_LEFT;
    } else if (bun->x + bun->box.left == 0) {
      bun->action = BTN_RIGHT;
    } else if (bun->y + bun->box.bottom == SCREEN_H) {
      bun->action = BTN_UP;
    } else if (bun->y + bun->box.top == 0) {
      bun->action = BTN_DOWN;
    } else if (RandomBetween(0, 1000) < 5) {
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

  TGA *grass = ResData(GetResource("grass.tga"));
  ShowImage(grass, 0, 0);

  TGA *tiles = ResData(GetResource("sprites.tga"));
  u8 *colors = (u8*)tiles->data;
  SetTiles(tiles);
  SetPalette(0, colors, tiles->paletteSize, tiles->paletteDepth);

  InitSprite(&gooseSprites[gooseIdle], Obj32x32, 512, 2, 20);
  InitSprite(&gooseSprites[gooseFlap], Obj32x32, 512+16*2, 4, 10);
  InitSprite(&gooseSprites[gooseRun], Obj32x32, 512+16*6, 4, 4);
  InitSprite(&gooseSprites[gooseWalk], Obj32x32, 512+16*10, 4, 8);

  Character goose;
  InitGoose(&goose, 0);

  Character buns[127];
  u32 numBuns = 3;

  for (u32 i = 0; i < numBuns; i++) {
    InitBun(&buns[i], i+1);
  }

  while (true) {
    VSync();

    u16 input = GetInput();

    UpdateCharacter(&goose, input);

    for (u32 i = 0; i < numBuns; i++) {
      UpdateCharacter(&buns[i], BunAI(&buns[i], &goose));

      if (numBuns < ArrayCount(buns) && buns[i].state == bunIdle && RandomBetween(0, 1000) < 1) {
        numBuns++;
        InitBun(&buns[numBuns-1], numBuns);
        PlaceCharacter(&buns[numBuns-1], buns[i].x, buns[i].y);
      }
    }
  }
}
