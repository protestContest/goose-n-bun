#include "res.h"
#include "math.h"
#include "str.h"
#include "mem.h"
#include "debug.h"

extern const ResFile __Resources__;
#define resFile (&__Resources__)

void *GetResource(char *name)
{
  u32 id = Hash(name, StrLen(name));
  for (u32 i = 0; i < resFile->count; i++) {
    if (resFile->resMap[i].id == id) {
      return ((u8*)resFile) + resFile->resMap[i].offset;
    }
  }
  return 0;
}

void *Uncompress(void *src, void *dst)
{
  if (!src) return 0;
  u32 header = *((u32*)src);
  u8 type = (header & 0xFF) >> 4;
  u32 size = header >> 8;

  if (!dst) dst = Alloc(size);

  if (type == Uncompressed) {
    Copy(ResData(src), dst, size);
  } else if (type == Huffman) {
    HuffUnComp((HuffData*)src, dst);
  } else if (type == LZSS) {
    LZSSUnComp((LZSSData*)src, dst);
  } else if (type == RunLength) {
    RLUnComp((RLData*)src, dst);
  } else if (type == SubFilter) {
    if ((header & 0x0F) == 1) {
      Diff8bitUnFilter(src, dst);
    } else if ((header & 0x0F) == 2) {
      Diff16bitUnFilter(src, dst);
    } else {
      Error("Invalid header");
    }
  } else {
    Error("Invalid header");
  }

  return dst;
}
