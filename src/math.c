#include "math.h"

u32 Hash(void *data, u32 size)
{
  u32 hash = 5381;
  u32 i;
  u8 *bytes = (u8*)data;
  for (i = 0; i < size; i++) {
    hash = ((hash << 5) + hash) + bytes[i];
  }
  return hash;
}

static volatile u32 randSeed = 1;

void SeedRandom(u32 seed)
{
  randSeed = seed;
  Random();
}

u32 Random(void)
{
  u32 hiSeed = randSeed >> 16;
  u32 loSeed = randSeed & 0xFFFF;
  u32 a, c;

  a = 0x41A7 * loSeed;
  c = 0x41A7 * hiSeed + (a >> 16);
  c = ((c & 0x7FFF) << 16) + ((2*c) >> 16);
  a = ((a & 0xFFFF) - 0x7FFFFFFF) + c;

  if ((i32)a < 0) a += 0x7FFFFFFF;
  randSeed = a;
  return randSeed;
}

u32 RandBetween(u32 low, u32 high)
{
  if (high < low) return RandBetween(high, low);
  u32 range = high - low;
  if (range == 0) return low;

  // Find smallest power of 2 >= range
  u32 mask = range - 1;
  mask |= mask >> 1;
  mask |= mask >> 2;
  mask |= mask >> 4;
  mask |= mask >> 8;
  mask |= mask >> 16;

  u32 r;
  do {
    r = Random() & mask;
  } while (r >= range);

  return r + low;
}
