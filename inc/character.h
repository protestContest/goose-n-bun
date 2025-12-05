#pragma once
#include "sprite.h"
#include "canvas.h"

struct Character;

typedef void (*CharacterUpdate)(struct Character *ch, u16 input);

typedef struct Character {
  u16 obj;
  Point pos;
  Rect box;
  Point spriteOffset;
  i16 state;
  u16 numStates;
  AnimatedSprite *sprites;
  u16 action;
  CharacterUpdate update;
} Character;

void PlaceCharacter(Character *ch, i16 x, i16 y);
void SetCharacterState(Character *ch, u16 targetState);
bool UpdateCharacterMovement(Character *ch, u16 input, i16 speed);
void UpdateCharacter(Character *ch, u16 input);
