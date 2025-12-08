#pragma once
#include "kit/sprite.h"
#include "kit/canvas.h"

struct Character;

typedef void (*CharacterUpdate)(struct Character *ch, u16 input);

enum {left, leftUp, up, rightUp, right, rightDown, down, leftDown};

typedef struct Character {
  u16 obj;
  Point pos;
  Rect box;
  Point spriteOffset;
  i16 state;
  u16 direction;
  AnimatedSprite *sprites;
  u16 action;
  CharacterUpdate update;
} Character;

void PlaceCharacter(Character *ch, i16 x, i16 y);
void SetCharacterSprite(Character *ch, u16 sprite);
bool UpdateCharacterMovement(Character *ch, u16 input, i16 speed);
void UpdateCharacter(Character *ch, u16 input);
