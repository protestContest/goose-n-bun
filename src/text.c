#include "text.h"
#include "canvas.h"
#include "res.h"
#include "debug.h"

#define propFont  (i16)0x9000
#define fixedFont (i16)0xB000
#define fontWid   (i16)0xACB0

typedef struct {
  i16 type;
  i16 first_char;
  i16 last_char;
  i16 wid_max;
  i16 kern_max;
  i16 neg_descent;
  i16 rect_width;
  i16 rect_height;
  i16 owt_offset;
  i16 ascent;
  i16 descent;
  i16 leading;
  i16 row_words;
} FontRec;

static FontRec *currentFont = 0;

static i16 DoString(char *str, FontRec *rec, bool draw)
{
  i16 *bit_image;
  i16 *loc_table;
  i16 *ow_table;
  i16 table_size;
  i16 ow;
  i16 font_type = rec->type & 0xFFFC;
  Pen pen;
  GetPen(&pen);
  i16 x = draw ? pen.pos.h : 0;
  i16 y = draw ? pen.pos.v : 0;

  table_size = rec->last_char - rec->first_char + 2;
  bit_image = (i16*)(rec + 1);
  loc_table = bit_image + rec->row_words*rec->rect_height;
  ow_table = &rec->owt_offset + rec->owt_offset;

  while (*str) {
    u8 offset, width;
    i16 loc, img_width, index, first_word, last_word, leading_bits, trailing_bits;

    /* find the table index for this char; default to the missing char */
    index = table_size - 1;
    if (*str >= rec->first_char && *str <= rec->last_char) {
      index = *str - rec->first_char;
    }

    ow = ow_table[index];
    if (ow == -1) {
      index = table_size - 1;
      ow = ow_table[index];
    }

    width = ow & 0xFF;
    offset = ow >> 8;
    loc = loc_table[index];
    img_width = loc_table[index+1] - loc;
    offset += rec->kern_max;

    first_word = loc / 16;
    last_word = (loc+img_width-1) / 16;
    leading_bits = loc % 16;
    trailing_bits = 15 - (loc+img_width-1) % 16;

    if (rec->type != fontWid && draw) {
      i32 bx, by, w;
      for (by = 0; by < rec->rect_height; by++) {
        i16 px = x - leading_bits + offset;
        i16 py = y - rec->ascent + by;
        for (w = first_word; w <= last_word; w++) {
          u16 word = bit_image[by*rec->row_words + w];
          if (w == first_word) word &= (0xFFFF >> leading_bits);
          if (w == last_word) word &= (0xFFFF << trailing_bits);
          if (!word) {
            px += 16;
            continue;
          }
          for (bx = 0; bx < 16; bx++) {
            if (word & (0x8000 >> bx)) {
              WritePixel(px, py, pen.color);
            }
            px++;
          }
        }
      }
    }

    if (font_type == propFont) {
      x += width;
    } else if (font_type == fixedFont) {
      x += rec->wid_max;
    }

    str++;
  }

  return x;
}

void SetFont(char *name)
{
  currentFont = GetResource(name);
  if (!currentFont) Error();
}

void GetFontInfo(FontInfo *info)
{
  if (!currentFont) return;
  info->ascent = currentFont->ascent;
  info->descent = currentFont->descent;
  info->widMax = currentFont->wid_max;
  info->leading = currentFont->leading;
  info->lineHeight = info->ascent + info->descent + info->leading;
}

void Print(char *str)
{
  if (!currentFont) return;
  i16 width = DoString(str, currentFont, true);
  Move(width, 0);
}

u32 StringWidth(char *str)
{
  if (!currentFont) return 0;
  return DoString(str, currentFont, false);
}
