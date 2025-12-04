#include "sprite.h"
#include "hardware.h"
#include "canvas.h"
#include "time.h"

void SetPalette(u32 n, u8 *colors, u32 numColors, u32 depth)
{
  u16 color = BLACK;
  for (u32 i = 0; i < Min(numColors, 16); i++) {
    if (depth == 16) {
      color = ((u16*)colors)[i];
    } else if (depth == 24) {
      color = RGB(colors[i*3+2], colors[i*3+1], colors[i*3]);
    } else if (depth == 32) {
      color = RGB(colors[i*4+2], colors[i*4+1], colors[i*4]);
    }

    SetPaletteColor(n, i, color);
  }
}

void SetPaletteColor(u32 palette, u32 index, u16 color)
{
  OBJ_PALETTE[16*palette + index] = color;
}

void InitObj(u32 obj)
{
  OAM[obj*4] = 0;
  OAM[obj*4+1] = 0;
  OAM[obj*4+2] = 0;
}

void PlaceObj(u32 obj, u16 x, u16 y)
{
  OAM[obj*4] = SetBits(OAM[obj*4], 0x00FF, y);
  OAM[obj*4+1] = SetBits(OAM[obj*4+1], 0x01FF, x);
}

void SetObjDisplay(u32 obj, u32 display)
{
  OAM[obj*4] = SetBits(OAM[obj*4], 0x0300, display << 8);
}

void SetObjMode(u32 obj, u32 mode)
{
  OAM[obj*4] = SetBits(OAM[obj*4], 0x0C00, mode << 10);
}

void SetObjMosaic(u32 obj, bool enable)
{
  OAM[obj*4] = SetBit(OAM[obj*4], 12, enable);
}

void SetObjColors(u32 obj, bool hicolor)
{
  OAM[obj*4] = SetBit(OAM[obj*4], 13, hicolor);
}

void SetObjFlip(u32 obj, u32 flip)
{
  OAM[obj*4+1] = SetBits(OAM[obj*4+1], 0x3000, flip << 12);
}

void SetObjSize(u32 obj, u32 size)
{
  OAM[obj*4] = SetBits(OAM[obj*4], 0xC000, size << 14);
  OAM[obj*4+1] = SetBits(OAM[obj*4+1], 0xC000, size << 12);
}

void SetObjSprite(u32 obj, u32 tile)
{
  OAM[obj*4+2] = SetBits(OAM[obj*4+2], 0x03FF, tile);
}

void SetObjPriority(u32 obj, u32 priority)
{
  OAM[obj*4+2] = SetBits(OAM[obj*4+2], 0x0C00, priority << 10);
}

void SetObjPalette(u32 obj, u32 palette)
{
  OAM[obj*4+2] = SetBits(OAM[obj*4+2], 0xF000, palette << 12);
}

void SetTiles(u8 *pixels, u32 width)
{
  u32 *tileWords = (u32*)CRAM;
  for (u32 tile = 0; tile < 32*16; tile++) {
    for (u32 tileRow = 0; tileRow < 8; tileRow++) {
      u8 *row = pixels + tile/32*width*8 + tileRow*width + (tile%32)*8;
      u32 word = 0;
      for (u32 i = 0; i < 8; i++) {
        word |= row[i] << 4*i;
      }

      tileWords[tile*8 + tileRow] = word;
    }
  }
}

void InitSprite(AnimatedSprite *sprite, u32 size, u32 baseTile, u32 numFrames, u32 speed)
{
  sprite->size = size;
  sprite->baseTile = baseTile;
  sprite->numFrames = numFrames;
  sprite->speed = speed;
  sprite->next = 0;
  sprite->frame = 0;
}

static AnimatedSprite *objSprites[128] = {0};

void AssignSprite(u32 obj, AnimatedSprite *sprite)
{
  SetObjSize(obj, sprite->size);
  SetObjSprite(obj, sprite->baseTile);
  objSprites[obj] = sprite;
  sprite->next = TickCount() + sprite->speed;
}

bool UpdateSprite(u32 obj)
{
  if (!objSprites[obj]) return false;
  u32 now = TickCount();
  AnimatedSprite *sprite = objSprites[obj];

  if (now > sprite->next) {
    sprite->next = now + sprite->speed;
    sprite->frame = (sprite->frame+1) % sprite->numFrames;
    SetObjSprite(obj, sprite->baseTile + 16*sprite->frame);
    return true;
  }

  return false;
}

void UpdateSprites(void)
{
  for (u32 i = 0; i < ArrayCount(objSprites); i++) {
    UpdateSprite(i);
  }
}
