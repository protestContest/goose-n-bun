#include "goose.h"
#include "kit.h"

static SpriteFrame gooseIdleFrames[] = {
  {512,0,0},
  {528,0,0}
};
static SpriteFrame gooseFlapFrames[] = {
  {544,0,0},
  {560,0,0},
  {576,0,0},
  {592,0,0}
};
static SpriteFrame gooseRunFrames[] = {
  {608,0,0},
  {624,0,0},
  {640,0,0},
  {656,0,0}
};
static SpriteFrame gooseWalkFrames[] = {
  {672,0,0},
  {688,0,0},
  {704,0,0},
  {720,0,0}
};
static SpriteFrame gooseWalkUpFrames[] = {
  {784,-1,0},
  {792,0,0},
  {784,1,0},
  {792,0,0}
};
static SpriteFrame gooseWalkDownFrames[] = {
  {824,-1,0},
  {816,0,0},
  {824,1,0},
  {816,0,0}
};
static SpriteFrame gooseRunUpFrames[] = {
  {800,-1,0},
  {808,0,0},
  {800,1,0},
  {808,0,0}
};
static SpriteFrame gooseRunDownFrames[] = {
  {840,-1,0},
  {832,0,0},
  {840,1,0},
  {832,0,0}
};

static AnimatedSprite gooseSprites[8];

void InitGoose(Character *goose, u32 obj)
{
  InitSprite(&gooseSprites[gooseIdle],    Obj32x32, 20, ArrayCount(gooseIdleFrames), gooseIdleFrames);
  InitSprite(&gooseSprites[gooseFlap],    Obj32x32, 10, ArrayCount(gooseFlapFrames), gooseFlapFrames);
  InitSprite(&gooseSprites[gooseRun],     Obj32x32, 4, ArrayCount(gooseRunFrames), gooseRunFrames);
  InitSprite(&gooseSprites[gooseWalk],    Obj32x32, 8, ArrayCount(gooseWalkFrames), gooseWalkFrames);
  InitSprite(&gooseSprites[gooseRunUp],   Obj16x32, 4, ArrayCount(gooseRunUpFrames), gooseRunUpFrames);
  InitSprite(&gooseSprites[gooseRunDown], Obj16x32, 4, ArrayCount(gooseRunDownFrames), gooseRunDownFrames);
  InitSprite(&gooseSprites[gooseWalkUp],  Obj16x32, 8, ArrayCount(gooseWalkUpFrames), gooseWalkUpFrames);
  InitSprite(&gooseSprites[gooseWalkDown],Obj16x32, 8, ArrayCount(gooseWalkDownFrames), gooseWalkDownFrames);

  goose->pos.h = SCREEN_W/2;
  goose->pos.v = SCREEN_H - 32;
  goose->obj = obj;
  goose->direction = left;
  goose->box.left = -5;
  goose->box.top = -5;
  goose->box.right = 5;
  goose->box.bottom = 0;
  goose->spriteOffset.h = -16;
  goose->spriteOffset.v = -32;
  goose->state = 0;
  goose->sprites = gooseSprites;
  goose->action = 0;
  goose->update = UpdateGoose;
  AssignSprite(goose->obj, &goose->sprites[0]);
  PlaceCharacter(goose, goose->pos.h, goose->pos.v);
}

void UpdateGoose(Character *ch, u16 input)
{
  i16 speed = (ch->state == gooseRun) ? 5 : 3;
  bool moved = UpdateCharacterMovement(ch, input, speed);

  if (moved && (input & BTN_B)) {
    ch->state = gooseRun;
  } else if (moved) {
    ch->state = gooseWalk;
  } else if (input & BTN_B) {
    ch->state = gooseFlap;
  } else {
    ch->state = gooseIdle;
  }

  if (!moved) {
    if (ch->state == gooseFlap) {
      SetCharacterSprite(ch, gooseFlap);
    } else {
      SetCharacterSprite(ch, gooseIdle);
    }
  } else {
    switch (ch->direction) {
    case left:
    case leftUp:
    case leftDown:
      if (ch->state == gooseRun) {
        SetCharacterSprite(ch, gooseRun);
      } else {
        SetCharacterSprite(ch, gooseWalk);
      }
      SetObjFlipH(ch->obj, true);
      break;
    case right:
    case rightUp:
    case rightDown:
      if (ch->state == gooseRun) {
        SetCharacterSprite(ch, gooseRun);
      } else {
        SetCharacterSprite(ch, gooseWalk);
      }
      SetObjFlipH(ch->obj, false);
      break;
    case up:
      if (ch->state == gooseRun) {
        SetCharacterSprite(ch, gooseRunUp);
      } else {
        SetCharacterSprite(ch, gooseWalkUp);
      }
      break;
    case down:
      if (ch->state == gooseRun) {
        SetCharacterSprite(ch, gooseRunDown);
      } else {
        SetCharacterSprite(ch, gooseWalkDown);
      }
      break;
    }
  }
}
