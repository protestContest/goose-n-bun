#include "kit/canvas.h"
#include "kit/mem.h"
#include "kit/hardware.h"
#include "kit/math.h"

static PenState pen = {{0,0}, {1,1}, 0};

Rect screenRect = {0, 0, SCREEN_W, SCREEN_H};

void ClearScreen(u16 color)
{
  u16 src[2] = {color, color};
  BlockFill((u32*)src, (u32*)VRAM, VRAM_WORDS);
}

Point GetPen(void)
{
  return pen.pos;
}

void GetPenState(PenState *p)
{
  *p = pen;
}

void PenSize(u16 h, u16 v)
{
  pen.size.h = h;
  pen.size.v = v;
}

void SetColor(u16 color)
{
  pen.color = color;
}

void MoveTo(i16 x, i16 y)
{
  pen.pos.h = x;
  pen.pos.v = y;
}

void Move(i16 dx, i16 dy)
{
  pen.pos.h += dx;
  pen.pos.v += dy;
}

void LineTo(i16 x1, i16 y1)
{
  /* Zingl 2012 */

  i16 x0 = pen.pos.h;
  i16 y0 = pen.pos.v;

  i16 dx = abs(x1 - x0);
  i16 dy = -abs(y1 - y0);
  i16 step_x = (x0 < x1) ? 1 : -1;
  i16 step_y = (y0 < y1) ? 1 : -1;
  i16 err = dx + dy;
  i16 e2;

  while (true) {
    for (i16 py = 0; py < pen.size.v; py++) {
      for (i16 px = 0; px < pen.size.h; px++) {
        WritePixel(x0 + px, y0 + py, pen.color);
      }
    }
    e2 = 2*err;
    if (e2 >= dy) {
      if (x0 == x1) break;
      err += dy;
      x0 += step_x;
    }
    if (e2 <= dx) {
      if (y0 == y1) break;
      err += dx;
      y0 += step_y;
    }
  }

  MoveTo(x1, y1);
}

void Line(i16 dx, i16 dy)
{
  LineTo(pen.pos.h + dx, pen.pos.v + dy);
}

void SetRect(Rect *rect, i16 left, i16 top, i16 right, i16 bottom)
{
  rect->left = left;
  rect->top = top;
  rect->right = right;
  rect->bottom = bottom;
}

void FillRect(Rect *rect, u16 color)
{
  i16 x, y;
  for (y = rect->top; y < rect->bottom; y++) {
    for (x = rect->left; x < rect->right; x++) {
      WritePixel(x, y, color);
    }
  }
}

void FrameRect(Rect *rect)
{
  MoveTo(rect->left, rect->top);
  LineTo(rect->right - pen.size.h, rect->top);
  LineTo(rect->right - pen.size.h, rect->bottom - pen.size.v);
  LineTo(rect->left, rect->bottom - pen.size.v);
  LineTo(rect->left, rect->top);
}

void SectRect(Rect *rect, Rect *obj)
{
  rect->left = Max(rect->left, obj->left);
  rect->right = Min(rect->right, obj->right);
  rect->top = Max(rect->top, obj->top);
  rect->bottom = Min(rect->bottom, obj->bottom);
}

void OffsetRect(Rect *rect, i16 x, i16 y)
{
  rect->left += x;
  rect->right += x;
  rect->top += y;
  rect->bottom += y;
}

bool PtInRect(Rect *rect, Point p)
{
  return p.h >= rect->left && p.h < rect->right && p.v >= rect->top && p.v < rect->bottom;
}

i16 PtToAngle(Rect *r, Point pt)
{
  return 0;
}

void FrameArc(Rect *rect, i16 startAngle, i16 arcAngle)
{
}

void FillArc(Rect *rect, i16 startAngle, i16 arcAngle, u16 color)
{
}

void FrameRoundRect(Rect *rect, i16 ovalWidth, i16 ovalHeight)
{
}

void FillRoundRect(Rect *rect, i16 ovalWidth, i16 ovalHeight, u16 color)
{
}

void WritePixel(i16 x, i16 y, u16 color)
{
  PixelAt(x, y) = color;
}

void ShowImage(TGA *image, i16 x, i16 y)
{
  if (!image) return;

  for (u32 i = 0; i < image->height; i++) {
    for (u32 j = 0; j < image->width; j++) {
      u16 pixel = image->data[i * image->width + j];
      if (!(pixel & 0x8000)) continue;
      pixel = ((pixel & 0x1F) << 10) | (pixel & (0x1F << 5)) | ((pixel >> 10) & 0x1F);
      WritePixel(x + j, y + i, pixel);
    }
  }
}
