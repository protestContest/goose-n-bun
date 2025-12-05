#pragma once
#include "canvas.h"

typedef struct {
  u32 size;
  u32 baseTile;
  u32 numFrames;
  u32 speed;
  u32 next;
  u32 frame;
} AnimatedSprite;

void SetPalette(u32 n, u8 *colors, u32 numColors, u32 depth);
void SetPaletteColor(u32 palette, u32 index, u16 color);

void InitObj(u32 obj);
void PlaceObj(u32 obj, u16 x, u16 y);

#define ShowObj   0x0
#define TfmObj    0x1
#define HideObj   0x2
#define TfmDblObj 0x3
void SetObjDisplay(u32 obj, u32 display);

#define ObjModeNormal 0x0
#define ObjModeAlpha  0x1
#define ObjModeWindow 0x2
void SetObjMode(u32 obj, u32 mode);
void SetObjMosaic(u32 obj, bool enable);
void SetObjColors(u32 obj, bool hicolor);

void SetObjFlipH(u32 obj, bool flip);
void SetObjFlipV(u32 obj, bool flip);

#define Obj8x8    0x0
#define Obj16x16  0x4
#define Obj32x32  0x8
#define Obj64x64  0xC
#define Obj16x8   0x1
#define Obj32x8   0x5
#define Obj32x16  0x9
#define Obj64x32  0xD
#define Obj8x16   0x2
#define Obj8x32   0x6
#define Obj16x32  0xA
#define Obj32x64  0xE
void SetObjSize(u32 obj, u32 size);

void SetObjSprite(u32 obj, u32 tile);
void SetObjPriority(u32 obj, u32 priority);
void SetObjPalette(u32 obj, u32 palette);

void SetTiles(TGA *tga);

void InitSprite(AnimatedSprite *sprite, u32 size, u32 baseTile, u32 numFrames, u32 speed);
void AssignSprite(u32 obj, AnimatedSprite *sprite);
bool UpdateSprite(u32 obj);
void UpdateSprites(void);
