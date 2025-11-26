#pragma once

extern void Reset(void);

/*
numWords is rounded up to a multiple of 8
numWords is a 20-bit uint
src and dst must be word aligned
*/
extern void BlockFill(u32 *src, u32 *dst, u32 numWords);
extern void BlockCopy(u32 *src, u32 *dst, u32 numWords);

/*
initial is usually 0xFFFF
src and length should be aligned to 2 bytes
*/
extern u16 Checksum(u16 initial, void *src, u32 length);

extern void Halt(void);
extern void Stop(void);
extern void VBlankIntrWait(void);
