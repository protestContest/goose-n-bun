#include "canvas.h"

static Pen pen = {0};

Rect screenRect = {0, 0, SCREEN_W, SCREEN_H};

void GetPen(Pen *p)
{
  *p = pen;
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
    WritePixel(x0, y0, pen.color);
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
  LineTo(rect->right - 1, rect->top);
  LineTo(rect->right - 1, rect->bottom - 1);
  LineTo(rect->left, rect->bottom - 1);
  LineTo(rect->left, rect->top);
}

void RoundRect(Rect *rect)
{
  MoveTo(rect->left, rect->top);
  LineTo(rect->right - 1, rect->top);
  MoveTo(rect->right - 1, rect->top);
  LineTo(rect->right - 1, rect->bottom - 1);
  MoveTo(rect->right - 1, rect->bottom - 1);
  LineTo(rect->left, rect->bottom - 1);
  MoveTo(rect->left, rect->bottom - 1);
  LineTo(rect->left, rect->top);
}

void WritePixel(i16 x, i16 y, u16 color)
{
  // if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
    PixelAt(x, y) = color;
  // }
}
