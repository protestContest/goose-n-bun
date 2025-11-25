#pragma once
#include "hardware.h"

typedef struct {
  i16 h;
  i16 v;
} Point;

typedef struct {
  i16 left;
  i16 top;
  i16 right;
  i16 bottom;
} Rect;

typedef struct {
  Point pos;
  u16 color;
} Pen;

extern Rect screenRect;

#define SCREEN_W 240
#define SCREEN_H 160

#define PixelAt(x, y)     VRAM[(x) + (y)*SCREEN_W]
#define RGB(r, g, b) ((r) & 0x1F) | (((g) & 0x1F) << 5) | (((b) & 0x1F) << 10)
#define BLACK RGB(0, 0, 0)
#define WHITE RGB(31, 31, 31)
#define RED RGB(31, 0, 0)
#define GREEN RGB(0, 31, 0)
#define BLUE RGB(0, 0, 31)

void GetPen(Pen *pen);
void SetColor(u16 color);
void MoveTo(i16 x, i16 y);
void Move(i16 dx, i16 dy);
void LineTo(i16 x, i16 y);
void Line(i16 dx, i16 dy);
void FillRect(Rect *rect, u16 color);
void FrameRect(Rect *rect);
void RoundRect(Rect *rect);
void WritePixel(i16 x, i16 y, u16 color);
