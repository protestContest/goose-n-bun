#include "mem.h"

extern i32 __HEAP_START__[];
extern i32 __HEAP_END__[];

static void *Seek(i32 *cur, i32 blockSize, i32 size);
static void *Gather(i32 *cur, i32 blockSize, i32 size);

static void *Seek(i32 *cur, i32 blockSize, i32 size)
{
  do {
    cur += blockSize;
    blockSize = *cur++;
  } while (blockSize >= 0 && cur < __HEAP_END__);
  if (cur == __HEAP_END__) return 0;

  return Gather(cur, blockSize, size);
}

static void *Gather(i32 *cur, i32 blockSize, i32 size)
{
  i32 *start = cur;
  i32 freeSize = -blockSize;
  while (freeSize < size && cur < __HEAP_END__) {
    cur += freeSize;
    blockSize = *cur++;
    if (blockSize >= 0) {
      return Seek(cur, blockSize, size);
    }
  }
  if (cur == __HEAP_END__) return 0;

  start[-1] = size;
  if (freeSize > size) {
    start[size] = size - freeSize;
  }
  return start;
}

void *Alloc(i32 size)
{
  i32 *cur = __HEAP_START__;
  i32 blockSize = *cur++;
  return Seek(cur, blockSize, size);
}

void Free(void *ptr)
{
  u32 *cur = (u32*)ptr;
  cur[-1] = -cur[-1];
}
