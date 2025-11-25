#pragma once

typedef struct {
  i16 ascent;
  i16 descent;
  i16 widMax;
  i16 leading;
  i16 lineHeight;
} FontInfo;

void SetFont(char *name);
void GetFontInfo(FontInfo *info);
void Print(char *str);
u32 StringWidth(char *str);
