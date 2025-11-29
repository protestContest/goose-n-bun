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


  if ((header & 0xFF) == 0) {
    return data + sizeof(header);
  }

  if (flags & FilterFlag) {
    u8 *dst = Alloc(length + sizeof(header));
    *((u32*)dst) = header;

    if ((flags & CompTypeMask) == Huffman) {
      HuffUnComp((HuffData*)data, dst + sizeof(header));
    } else if ((flags & CompTypeMask) == LZ77) {
      LZ77UnComp((LZ77Data*)data, dst + sizeof(header));
    } else if ((flags & CompTypeMask) == RunLength) {
      RLUnComp((RLData*)data, dst + sizeof(header));
    } else {
      Copy(data + sizeof(header), dst + sizeof(header), length);
    }

    u8 *dst2 = Alloc(length);

    if (flags & Filter8Bit) {
      Diff8bitUnFilter(dst, dst2);
    } else {
      Diff16bitUnFilter(dst, dst2);
    }

    Free(dst);
    return dst2;
  } else {
    u8 *dst = Alloc(length);

    if ((flags & CompTypeMask) == Huffman) {
      HuffUnComp((HuffData*)data, dst);
    } else if ((flags & CompTypeMask) == LZ77) {
      LZ77UnComp((LZ77Data*)data, dst);
    } else if ((flags & CompTypeMask) == RunLength) {
      RLUnComp((RLData*)data, dst);
    } else {
      Copy(data + sizeof(header), dst, length);
    }

    return dst;
  }

  return 0;
}
