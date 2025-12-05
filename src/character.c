#include "character.h"
#include "input.h"

void PlaceCharacter(Character *ch, i16 x, i16 y)
{
  ch->pos.h = x;
  ch->pos.v = y;
  PlaceObj(ch->obj, ch->pos.h + ch->spriteOffset.h, ch->pos.v + ch->spriteOffset.v);
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
      speedX = Min(ch->pos.h+ch->box.left, speedX);
      if (speedX > 0) {
        if (changed) {
          ch->pos.h -= speedX;
        }
        moved = true;
      }
    } else if (input & BTN_RIGHT) {
      speedX = Min(SCREEN_W-ch->pos.h-ch->box.right, speedX);
      if (speedX > 0) {
        if (changed) {
          ch->pos.h += speedX;
        }
        moved = true;
      }
    }

    i16 speedY = speed;
    if (input & BTN_UP) {
      speedY = Min(ch->pos.v+ch->box.top, speedY);
      if (speedY > 0) {
        if (changed) {
          ch->pos.v -= speedY;
        }
        moved = true;
      }
    } else if (input & BTN_DOWN) {
      speedY = Min(SCREEN_H-ch->pos.v-ch->box.bottom, speedY);
      if (speedY > 0) {
        if (changed) {
          ch->pos.v += speedY;
        }
        moved = true;
      }
    }

    PlaceCharacter(ch, ch->pos.h, ch->pos.v);
  }

  return moved;
}

void UpdateCharacter(Character *ch, u16 input)
{
  ch->update(ch, input);
}
