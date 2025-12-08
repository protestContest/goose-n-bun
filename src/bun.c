#include "bun.h"
#include "kit.h"
#include "goose.h"

static SpriteFrame bunIdleFrames[] = {
  {736,0,-1},
  {740,0,-1}
};
static SpriteFrame bunHopFrames[] = {
  {744,0,-1},
  {748,0,-1},
  {752,0,-1},
  {756,0,-1},
};
static SpriteFrame bunHopUpFrames[] = {
  {760,0,-1},
  {764,0,-1},
  {768,0,-1},
  {772,0,-1},
};
static SpriteFrame bunHopDownFrames[] = {
  {760,0,1},
  {764,0,1},
  {768,0,1},
  {772,0,1},
};

void InitBun(Character *bun, u32 obj)
{
  bun->pos.h = RandomBetween(10, SCREEN_W - 26);
  bun->pos.v = RandomBetween(10, SCREEN_H - 26);
  bun->obj = obj;
  bun->box.left = -5;
  bun->box.top = -5;
  bun->box.right = 5;
  bun->box.bottom = 0;
  bun->spriteOffset.h = -8;
  bun->spriteOffset.v = -16;
  bun->state = 0;
  bun->sprites = Alloc(sizeof(AnimatedSprite)*4);
  InitSprite(&bun->sprites[bunIdle], Obj16x16, 18, ArrayCount(bunIdleFrames), bunIdleFrames);
  InitSprite(&bun->sprites[bunHop], Obj16x16, 8, ArrayCount(bunHopFrames), bunHopFrames);
  InitSprite(&bun->sprites[bunHopUp], Obj16x16, 8, ArrayCount(bunHopUpFrames), bunHopUpFrames);
  InitSprite(&bun->sprites[bunHopDown], Obj16x16, 8, ArrayCount(bunHopDownFrames), bunHopDownFrames);

  bun->action = 0;
  bun->update = UpdateBun;
  AssignSprite(bun->obj, &bun->sprites[0]);
  PlaceCharacter(bun, bun->pos.h, bun->pos.v);
  SetObjDisplay(bun->obj, ShowObj);
}

void DestroyBun(Character *bun)
{
  SetObjDisplay(bun->obj, HideObj);
  Free(bun->sprites);
  bun->state = -1;
}

void UpdateBun(Character *ch, u16 input)
{
  bool moved = UpdateCharacterMovement(ch, input, 4);

  if (moved) {
    if (input & (BTN_LEFT | BTN_RIGHT)) {
      ch->state = bunHop;
    } else if (input & BTN_UP) {
      ch->state = bunHopUp;
    } else {
      ch->state = bunHopDown;
    }
  } else {
    ch->state = bunIdle;
  }

  if (input & BTN_RIGHT) {
    SetObjFlipH(ch->obj, true);
  } else if (input & BTN_LEFT) {
    SetObjFlipH(ch->obj, false);
  }

  SetCharacterSprite(ch, ch->state);
}

u16 TurnLeft(u16 keys)
{
  switch (keys) {
  case BTN_UP:
    return BTN_LEFT | BTN_UP;
  case BTN_UP | BTN_RIGHT:
    return BTN_UP;
  case BTN_RIGHT:
    return BTN_UP | BTN_RIGHT;
  case BTN_RIGHT | BTN_DOWN:
    return BTN_RIGHT;
  case BTN_DOWN:
    return BTN_RIGHT | BTN_DOWN;
  case BTN_DOWN | BTN_LEFT:
    return BTN_DOWN;
  case BTN_LEFT:
    return BTN_DOWN | BTN_LEFT;
  case BTN_LEFT | BTN_UP:
    return BTN_LEFT;
  default:
    return 0;
  }
}

u16 TurnRight(u16 keys)
{
  switch (keys) {
  case BTN_UP:
    return BTN_RIGHT | BTN_UP;
  case BTN_UP | BTN_RIGHT:
    return BTN_RIGHT;
  case BTN_RIGHT:
    return BTN_DOWN | BTN_RIGHT;
  case BTN_RIGHT | BTN_DOWN:
    return BTN_DOWN;
  case BTN_DOWN:
    return BTN_LEFT | BTN_DOWN;
  case BTN_DOWN | BTN_LEFT:
    return BTN_LEFT;
  case BTN_LEFT:
    return BTN_UP | BTN_LEFT;
  case BTN_LEFT | BTN_UP:
    return BTN_UP;
  default:
    return 0;
  }
}

u16 BunAI(Character *bun, Character *goose)
{
  if (goose->state == gooseFlap || goose->state == gooseRun) {
    i16 tooClose = 40;
    i16 dx = goose->pos.h - bun->pos.h;
    i16 dy = goose->pos.v - bun->pos.v;
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

      if (bun->pos.h + bun->box.right >= SCREEN_W - 4 ||
          bun->pos.h + bun->box.left <= 4) {
        if (dy > 0) {
          bun->action |= BTN_UP;
        } else {
          bun->action |= BTN_DOWN;
        }
      } else if (bun->pos.v + bun->box.bottom >= SCREEN_H - 4 ||
          bun->pos.v + bun->box.top <= 4) {
        if (dx > 0) {
          bun->action |= BTN_LEFT;
        } else {
          bun->action |= BTN_RIGHT;
        }
      } else {
        u32 r = RandomBetween(0, 1000);
        if (r < 20) {
          bun->action = TurnLeft(bun->action & BTN_DPAD);
        } else if (r < 40) {
          bun->action = TurnRight(bun->action & BTN_DPAD);
        }
      }

      return bun->action;
    }
  }

  if (bun->state == bunIdle) {
    if (bun->pos.h + bun->box.right == SCREEN_W) {
      bun->action = BTN_LEFT;
    } else if (bun->pos.h + bun->box.left == 0) {
      bun->action = BTN_RIGHT;
    } else if (bun->pos.v + bun->box.bottom == SCREEN_H) {
      bun->action = BTN_UP;
    } else if (bun->pos.v + bun->box.top == 0) {
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
      u32 r = RandomBetween(0, 1000);
      if (r < 10) {
        bun->action = 0;
      } else if (r < 20) {
        bun->action = TurnLeft(bun->action & BTN_DPAD);
      } else if (r < 30) {
        bun->action = TurnRight(bun->action & BTN_DPAD);
      }
  }
  return bun->action;
}
