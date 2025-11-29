#include "res.h"
#include "math.h"
#include "str.h"
#include "compress.h"
#include "mem.h"
#include "debug.h"

extern const ResFile __Resources__;
#define resFile (&__Resources__)

void *RawResource(char *name)
{
  u32 id = Hash(name, StrLen(name));

  for (u32 i = 0; i < resFile->count; i++) {
    if (resFile->resMap[i].id == id) {
      u8 *src = ((u8*)resFile) + resFile->resMap[i].offset;
      return src;
    }
  }

  return 0;
}

u32 ResLength(char *name)
{
  u32 *data = RawResource(name);
  if (!data) return 0;
  return data[0] >> 8;
}

void *GetResource(char *name)
{
  u8 *data = RawResource(name);
  if (!data) return 0;

  u32 header = *((u32*)data);
  u8 flags = header & 0xFF;
  u32 length = header >> 8;
  data += 4;

  if ((header & 0xFF) == 0) {
    return data;
  }

  u8 *dst = Alloc(length);

  if (flags & Huffman4Bit) {
    HuffUnComp((HuffData*)data, dst);
  } else if (flags & Huffman8Bit) {
    HuffUnComp((HuffData*)data, dst);
  } else if (flags & LZ77) {
    LZ77UnComp((LZ77Data*)data, dst);
  } else if (flags & RunLength) {
    RLUnComp((RLData*)data, dst);
  }

  if (flags & Filter8Bit) {
    Diff8bitUnFilter(dst, dst);
  } else if (flags & Filter16Bit) {
    Diff16bitUnFilter(dst, dst);
  }

  return dst;
}
