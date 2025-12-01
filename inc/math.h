#pragma once

// BIOS
extern u16 Sqrt(u32 n);
extern u16 Atan(u16 tan);
extern u16 Atan2(u16 x, u16 y);

#define MAX_UINT  0xFFFFFFFF
#define MAX_INT   0x7FFFFFFF
#define MIN_INT   0x80000000

u32 Hash(void *data, u32 size);
void SeedRandom(u32 seed);
u32 Random(void);
u32 RandBetween(u32 low, u32 high);

i32 Sin(i16 angle);
#define Cos(angle) Sin((angle) + 90)
