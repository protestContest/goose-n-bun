#include "mem.h"

/*
The heap is divided into allocated blocks. Each block begins with a 32-bit header. If the header is
positive, the block is allocated; if negative, the block is free. The absolute value of the header
is the block size in words (not including the header).

Allocation happens by first seeking to a free block, then gathering contiguous free blocks until
enough space is claimed. The last free block (if not completely claimed) is split, and the claimed
blocks are marked as one allocated block.
*/

extern i32 __HEAP_START__[];
extern i32 __HEAP_END__[];

static void Gather(i32 *cur)
{
  i32 *start = cur;
  i32 blockSize = cur[-1];
  i32 freeSize = 0;

  while (blockSize <= 0) {
    freeSize += -blockSize + 1;
    cur += -blockSize;
    if (cur >= __HEAP_END__) break;
    blockSize = *cur++;
  }

  start[-1] = -(freeSize - 1);
}

static void *Seek(i32 *cur, i32 size)
{
  while (cur < __HEAP_END__ && cur[-1] > 0) {
    cur += cur[-1] + 1;
  }
  if (cur >= __HEAP_END__) return 0;

  Gather(cur);

  if (-cur[-1] < size) {
    cur += -cur[-1] + 1;
    return Seek(cur, size);
  }

  return cur;
}

void *Alloc(i32 size)
{
  i32 *ptr = __HEAP_START__;
  i32 words = Align(size, 4) >> 2;

  ptr = Seek(ptr, words);
  if (!ptr) return 0;

  if (-ptr[-1] > words) {
    ptr[words] = ptr[-1] + words;
  }

  ptr[-1] = words;

  return ptr;
}

void Free(void *ptr)
{
  i32 *cur = (i32*)ptr;
  if (cur < __HEAP_START__ || cur >= __HEAP_END__) return;
  cur[-1] = -cur[-1];
  Gather(cur);
}

void Copy(void *src, void *dst, u32 size)
{
  u8 *srcBytes = (u8*)src;
  u8 *dstBytes = (u8*)dst;

  if (size >= 8 && (u32)srcBytes == Align(srcBytes, 4) && (u32)dstBytes == Align(dstBytes, 4)) {
    BlockCopy(srcBytes, dstBytes, size/4);
    srcBytes += size/4;
    dstBytes += size/4;
    for (u32 i = 0; i < size%4; i++) {
      *dstBytes++ = *srcBytes++;
    }
    return;
  }

  for (u32 i = 0; i < size; i++) *dstBytes++ = *srcBytes++;
}
