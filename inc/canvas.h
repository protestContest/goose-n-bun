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
  Point size;
  u16 color;
} PenState;

extern Rect screenRect;

#define SCREEN_W 240
#define SCREEN_H 160

#define PixelAt(x, y)     VRAM[(x) + (y)*SCREEN_W]
#define RGB(r, g, b) ((((r)>>3) & 0x1F) | ((((g)>>3) & 0x1F) << 5) | ((((b)>>3) & 0x1F) << 10))
#define BLACK RGB(0, 0, 0)
#define WHITE RGB(255, 255, 255)
#define RED RGB(255, 0, 0)
#define GREEN RGB(0, 255, 0)
#define BLUE RGB(0, 0, 255)

void ClearScreen(u16 color);

Point GetPen(void);
void GetPenState(PenState *pen);
void PenSize(u16 h, u16 v);
void SetColor(u16 color);
void MoveTo(i16 x, i16 y);
void Move(i16 dx, i16 dy);
void LineTo(i16 x, i16 y);
void Line(i16 dx, i16 dy);
void SetRect(Rect *rect, i16 left, i16 top, i16 right, i16 bottom);
void FillRect(Rect *rect, u16 color);
void FrameRect(Rect *rect);

i16 PtToAngle(Rect *r, Point pt);
void FrameArc(Rect *rect, i16 startAngle, i16 arcAngle);
void FillArc(Rect *rect, i16 startAngle, i16 arcAngle, u16 color);

void FrameRoundRect(Rect *rect, i16 ovalWidth, i16 ovalHeight);
void FillRoundRect(Rect *rect, i16 ovalWidth, i16 ovalHeight, u16 color);

void WritePixel(i16 x, i16 y, u16 color);
