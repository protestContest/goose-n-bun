#include "res.h"
#include "math.h"
#include "str.h"
#include "compress.h"
#include "mem.h"
#include "debug.h"

extern const ResFile __Resources__;
#define resFile (&__Resources__)

void *GetResource(char *name)
{
  u32 id = Hash(name, StrLen(name));

  for (u32 i = 0; i < resFile->count; i++) {
    if (resFile->resMap[i].id == id) {
      u8 *src = ((u8*)resFile) + resFile->resMap[i].offset;
      u32 header = *((u32*)src);

      if ((header & 0xFF) == 0) {
        return src + sizeof(header);
      }

      u8 *dst = Alloc(header >> 8);

      if (header & Huffman4Bit) {
        HuffUnComp((HuffData*)src, dst);
      } else if (header & Huffman8Bit) {
        HuffUnComp((HuffData*)src, dst);
      } else if (header & LZ77) {
        LZ77UnComp((LZ77Data*)src, dst);
      } else if (header & RunLength) {
        RLUnComp((RLData*)src, dst);
      }

      if (header & Filter8Bit) {
        Diff8bitUnFilter(dst, dst);
      } else if (header & Filter16Bit) {
        Diff16bitUnFilter(dst, dst);
      }

      return dst;
    }
  }

  return 0;
}
