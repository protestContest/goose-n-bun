#include "ui.h"
#include "canvas.h"
#include "text.h"

void ShowWindow(Rect *bounds)
{
  i16 x = bounds->left;
  i16 y = bounds->top;
  i16 width = bounds->right - bounds->left;
  i16 height = bounds->bottom - bounds->top;
  Rect bg = {x+7, y+7, x+width-7, y+height-7};
  FillRect(&bg, RGB(0, 96, 136));

  SetColor(BLACK);
  MoveTo(x, y+1);
  LineTo(x+1, y);
  MoveTo(x+width-2, y);
  LineTo(x+width-1, y+1);
  LineTo(x+width-1, y+height-2);
  LineTo(x+width-2, y+height-1);
  LineTo(x+1, y+height-1);
  LineTo(x, y+height-2);

  SetColor(RGB(80, 80, 80));
  MoveTo(x+width-2, y+height-2);
  LineTo(x+width-2, y+height-2);

  MoveTo(x+2, y);
  LineTo(x+width-3, y);
  LineTo(x+width-2, y+1);
  SetColor(RGB(160, 160, 160));
  LineTo(x+width-2, y+height-3);
  LineTo(x+width-3, y+height-3);
  LineTo(x+width-3, y+height-2);
  LineTo(x+2, y+height-2);
  LineTo(x+1, y+height-3);
  MoveTo(x+1, y+height-2);
  SetColor(RGB(80,80,80));
  LineTo(x, y+height-3);
  LineTo(x, y+2);
  LineTo(x+2, y);

  SetColor(RGB(248,248,248));
  MoveTo(x+2, y+1);
  LineTo(x+width-3, y+1);
  LineTo(x+width-3, y+height-4);
  MoveTo(x+width-4, y+height-3);
  LineTo(x+2, y+height-3);
  MoveTo(x+1, y+height-4);
  LineTo(x+1, y+2);

  SetColor(RGB(160, 160, 160));
  MoveTo(x+2,y+height-4);
  LineTo(x+2, y+2);
  LineTo(x+width-4, y+2);
  SetColor(RGB(80, 80, 80));
  MoveTo(x+width-4, y+3);
  LineTo(x+width-4, y+height-4);
  MoveTo(x+width-5, y+height-5);
  LineTo(x+width-5, y+height-4);
  LineTo(x+3, y+height-4);
  MoveTo(x+3, y+3);
  LineTo(x+3, y+3);

  SetColor(BLACK);
  MoveTo(x+4, y+height-5);
  LineTo(x+3, y+height-5);
  LineTo(x+3, y+4);
  LineTo(x+4, y+4);
  LineTo(x+4, y+3);
  LineTo(x+width-5, y+3);
  LineTo(x+width-5, y+4);

  SetColor(RGB(0,72,80));
  MoveTo(x+4, y+height-6);
  LineTo(x+4, y+5);
  MoveTo(x+5, y+4);
  LineTo(x+width-6, y+4);
  SetColor(RGB(0,80,96));
  MoveTo(x+5, y+height-6);
  LineTo(x+5, y+5);
  LineTo(x+width-6, y+5);
  SetColor(RGB(0, 88, 112));
  MoveTo(x+6, y+height-7);
  LineTo(x+6, y+6);
  LineTo(x+width-7, y+6);

  SetColor(RGB(8,128,184));
  MoveTo(x+width-5, y+5);
  LineTo(x+width-5, y+height-6);
  MoveTo(x+width-6, y+height-5);
  LineTo(x+5, y+height-5);
  SetColor(RGB(8, 112, 168));
  MoveTo(x+width-6, y+6);
  LineTo(x+width-6, y+height-6);
  LineTo(x+6, y+height-6);
  SetColor(RGB(8, 104, 152));
  MoveTo(x+width-7, y+7);
  LineTo(x+width-7, y+height-7);
  LineTo(x+7, y+height-7);
}

void TextWindow(char *text, i16 x, i16 y)
{
  Rect bounds;
  FontInfo finfo;
  GetFontInfo(&finfo);
  i16 width = TextWidth(text) + 16;
  i16 height = TextHeight(text) + 16;

  bounds.left = x;
  bounds.top = y;
  bounds.right = bounds.left+width;
  bounds.bottom = bounds.top+height;

  ShowWindow(&bounds);
  SetColor(BLACK);
  MoveTo(bounds.left + 9, bounds.top + 9 + finfo.ascent);
  Print(text);
  SetColor(WHITE);
  MoveTo(bounds.left + 8, bounds.top + 8 + finfo.ascent);
  Print(text);
}

void Alert(char *text)
{
  i16 width = TextWidth(text) + 16;
  i16 height = TextHeight(text) + 16;
  TextWindow(text, SCREEN_W/2 - width/2, SCREEN_H/2 - height/2);
}
