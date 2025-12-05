#pragma once
#include "character.h"

enum {gooseIdle, gooseFlap, gooseRun, gooseWalk};

void UpdateGoose(Character *ch, u16 input);
void InitGoose(Character *goose, u32 obj);
