#pragma once

/*
numWords is rounded up to a multiple of 8
numWords is a 20-bit uint
src and dst must be word aligned
*/
extern void BlockFill(void *src, void *dst, u32 numWords);
extern void BlockCopy(void *src, void *dst, u32 numWords);

#define Align(n, m)       (((u32)(n) + (u32)(m) - 1) & ~((u32)(m) - 1))

void *Alloc(i32 size);
void Free(void *ptr);
void Copy(void *src, void *dst, u32 length);