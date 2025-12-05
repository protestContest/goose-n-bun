#include "goose.h"
#include "input.h"

static AnimatedSprite gooseSprites[4];

void InitGoose(Character *goose, u32 obj)
{
  InitSprite(&gooseSprites[gooseIdle], Obj32x32, 512, 2, 20);
  InitSprite(&gooseSprites[gooseFlap], Obj32x32, 512+16*2, 4, 10);
  InitSprite(&gooseSprites[gooseRun], Obj32x32, 512+16*6, 4, 4);
  InitSprite(&gooseSprites[gooseWalk], Obj32x32, 512+16*10, 4, 8);

  goose->pos.h = SCREEN_W/2;
  goose->pos.v = SCREEN_H - 32;
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
  PlaceCharacter(goose, goose->pos.h, goose->pos.v);
}

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
  } else if (input & BTN_B) {
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
