#pragma once
#include "character.h"

enum {bunIdle, bunHop, bunHopVert};

void InitBun(Character *bun, u32 obj);
void DestroyBun(Character *bun);
void UpdateBun(Character *ch, u16 input);
u16 BunAI(Character *bun, Character *goose);
