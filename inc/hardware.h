#pragma once

// https://problemkaputt.de/gbatek.htm#gbamemorymap

// BIOS

extern void Reset(void);
extern void Halt(void);
extern void Stop(void);
extern void IntrWait(bool clearIF, u32 interrupts);
extern void VBlankIntrWait(void);



// LCD
#define REG_DISPCNT     *((volatile u16*)0x04000000)
#define GraphicsMode(n) do {\
  REG_DISPCNT = (n) & 7;\
} while (0)
#define Flip() do {\
  REG_DISPCNT = REG_DISPCNT ^ (1 << 4);\
} while (0)
#define HBlankFree(n) do {\
  REG_DISPCNT = (REG_DISPCNT & ~(1 << 5)) | ((n) << 5);\
} while (0)
#define ObjVRAMMapMode(n) do {\
  REG_DISPCNT = (REG_DISPCNT & ~(1 << 6)) | ((n) << 6);\
} while (0)
#define ForcedBlank(n) do {\
  REG_DISPCNT = (REG_DISPCNT & ~(1 << 7)) | ((n) << 7);\
} while (0)
#define DISP_BG0    (1 << 8)
#define DISP_BG1    (1 << 9)
#define DISP_BG2    (1 << 10)
#define DISP_BG3    (1 << 11)
#define DISP_OBJ    (1 << 12)
#define DISP_WIN1   (1 << 13)
#define DISP_WIN2   (1 << 14)
#define DISP_WINOBJ (1 << 15)
#define ShowLayer(n) do {\
  REG_DISPCNT |= (n);\
} while (0)
#define HideLayer(n) do {\
  REG_DISPCNT &= ~(n);\
} while (0)
#define ScreenOff() do {\
  REG_DISPCNT &= ~(0xFF00);\
} while (0)

#define REG_DISPSTAT    *((volatile u16*)0x04000004)
#define LCD_VBLANK_IRQ  (1 << 3)
#define LCD_HBLANK_IRQ  (1 << 4)
#define LCD_VCOUNT_IRQ  (1 << 5)
#define REG_VCOUNT      *((volatile u16*)0x04000006)
#define REG_BG0CNT      *((volatile u16*)0x04000008)
#define REG_BG1CNT      *((volatile u16*)0x0400000A)
#define REG_BG2CNT      *((volatile u16*)0x0400000C)
#define REG_BG3CNT      *((volatile u16*)0x0400000E)
#define REG_BG0HOFS     *((volatile u16*)0x04000010)
#define REG_BG0VOFS     *((volatile u16*)0x04000012)
#define REG_BG1HOFS     *((volatile u16*)0x04000014)
#define REG_BG1VOFS     *((volatile u16*)0x04000016)
#define REG_BG2HOFS     *((volatile u16*)0x04000018)
#define REG_BG2VOFS     *((volatile u16*)0x0400001A)
#define REG_BG3HOFS     *((volatile u16*)0x0400001C)
#define REG_BG3VOFS     *((volatile u16*)0x0400001E)
#define REG_BG2PA       *((volatile u16*)0x04000020)
#define REG_BG2PB       *((volatile u16*)0x04000022)
#define REG_BG2PC       *((volatile u16*)0x04000024)
#define REG_BG2PD       *((volatile u16*)0x04000026)
#define REG_BG2X        *((volatile u32*)0x04000028)
#define REG_BG2Y        *((volatile u32*)0x0400002C)
#define REG_BG3PA       *((volatile u16*)0x04000030)
#define REG_BG3PB       *((volatile u16*)0x04000032)
#define REG_BG3PC       *((volatile u16*)0x04000034)
#define REG_BG3PD       *((volatile u16*)0x04000036)
#define REG_BG3X        *((volatile u32*)0x04000038)
#define REG_BG3Y        *((volatile u32*)0x0400003C)
#define REG_WIN0H       *((volatile u16*)0x04000040)
#define REG_WIN1H       *((volatile u16*)0x04000042)
#define REG_WIN0V       *((volatile u16*)0x04000044)
#define REG_WIN1V       *((volatile u16*)0x04000046)
#define REG_WININ       *((volatile u16*)0x04000048)
#define REG_WINOUT      *((volatile u16*)0x0400004A)
#define REG_MOSAIC      *((volatile u16*)0x0400004C)
#define REG_BLDCNT      *((volatile u16*)0x04000050)
#define REG_BLDALPHA    *((volatile u16*)0x04000052)
#define REG_BLDY        *((volatile u16*)0x04000054)

// Mode 3
#define VRAM            ((u16*)0x06000000)
#define VRAM_END        ((u16*)0x06018000)
#define VRAM_WORDS      19200



// Sound
#define REG_SOUND1CNT_L *((volatile u16*)0x04000060)
#define REG_SOUND1CNT_H *((volatile u16*)0x04000062)
#define REG_SOUND1CNT_X *((volatile u16*)0x04000064)
#define REG_SOUND2CNT_L *((volatile u16*)0x04000068)
#define REG_SOUND2CNT_H *((volatile u16*)0x0400006C)
#define REG_SOUND3CNT_L *((volatile u16*)0x04000070)
#define REG_SOUND3CNT_H *((volatile u16*)0x04000072)
#define REG_SOUND3CNT_X *((volatile u16*)0x04000074)
#define REG_SOUND4CNT_L *((volatile u16*)0x04000078)
#define REG_SOUND4CNT_H *((volatile u16*)0x0400007C)
#define REG_SOUNDCNT_L  *((volatile u16*)0x04000080)
#define REG_SOUNDCNT_H  *((volatile u16*)0x04000082)
#define REG_SOUNDCNT_X  *((volatile u16*)0x04000084)
#define REG_SOUNDBIAS   *((volatile u16*)0x04000088)
#define REG_WAVE_RAM    *((volatile u16*)0x04000090)
#define REG_FIFO_A      *((volatile u16*)0x040000A0)
#define REG_FIFO_B      *((volatile u16*)0x040000A4)



// DMA
#define REG_DMA0SAD     *((volatile u16*)0x040000B0)
#define REG_DMA0DAD     *((volatile u16*)0x040000B4)
#define REG_DMA0CNT_L   *((volatile u16*)0x040000B8)
#define REG_DMA0CNT_H   *((volatile u16*)0x040000BA)
#define REG_DMA1SAD     *((volatile u16*)0x040000BC)
#define REG_DMA1DAD     *((volatile u16*)0x040000C0)
#define REG_DMA1CNT_L   *((volatile u16*)0x040000C4)
#define REG_DMA1CNT_H   *((volatile u16*)0x040000C6)
#define REG_DMA2SAD     *((volatile u16*)0x040000C8)
#define REG_DMA2DAD     *((volatile u16*)0x040000CC)
#define REG_DMA2CNT_L   *((volatile u16*)0x040000D0)
#define REG_DMA2CNT_H   *((volatile u16*)0x040000D2)
#define REG_DMA3SAD     *((volatile u16*)0x040000D4)
#define REG_DMA3DAD     *((volatile u16*)0x040000D8)
#define REG_DMA3CNT_L   *((volatile u16*)0x040000DC)
#define REG_DMA3CNT_H   *((volatile u16*)0x040000DE)



// Timers
#define REG_TM0CNT      *((volatile u32*)0x04000100)
#define REG_TM0CNT_L    *((volatile u16*)0x04000100)
#define REG_TM0CNT_H    *((volatile u16*)0x04000102)
#define REG_TM1CNT      *((volatile u32*)0x04000104)
#define REG_TM1CNT_L    *((volatile u16*)0x04000104)
#define REG_TM1CNT_H    *((volatile u16*)0x04000106)
#define REG_TM2CNT      *((volatile u32*)0x04000108)
#define REG_TM2CNT_L    *((volatile u16*)0x04000108)
#define REG_TM2CNT_H    *((volatile u16*)0x0400010A)
#define REG_TM3CNT      *((volatile u32*)0x0400010C)
#define REG_TM3CNT_L    *((volatile u16*)0x0400010C)
#define REG_TM3CNT_H    *((volatile u16*)0x0400010E)

#define SetTimeout(ms, fn)  do {\
  OnInterrupt(INT_TIMER3, fn);\
  REG_TM3CNT_L = (ms);\
  REG_TM3CNT_H = 0xC3;\
} while (0)



// Serial 1
#define REG_SIODATA32   *((volatile u16*)0x04000120)
#define REG_SIOMULTI0   *((volatile u16*)0x04000120)
#define REG_SIOMULTI1   *((volatile u16*)0x04000122)
#define REG_SIOMULTI2   *((volatile u16*)0x04000124)
#define REG_SIOMULTI3   *((volatile u16*)0x04000126)
#define REG_SIOCNT      *((volatile u16*)0x04000128)
#define REG_SIOMLT_SEND *((volatile u16*)0x0400012A)
#define REG_SIODATA8    *((volatile u16*)0x0400012A)



// Keypad
#define REG_KEYINPUT    *((volatile u16*)0x04000130)
#define REG_KEYCNT      *((volatile u16*)0x04000132)



// Serial 2
#define REG_RCNT        *((volatile u16*)0x04000134)
#define REG_IR          *((volatile u16*)0x04000136)
#define REG_JOYCNT      *((volatile u16*)0x04000140)
#define REG_JOY_RECV    *((volatile u16*)0x04000150)
#define REG_JOY_TRANS   *((volatile u16*)0x04000154)
#define REG_JOYSTAT     *((volatile u16*)0x04000158)



// Interrupts
#define REG_IME         *((volatile u16*)0x04000208)
#define REG_IE          *((volatile u16*)0x04000200)
#define REG_IF          *((volatile u16*)0x04000202)
#define REG_BIOSIF      *((volatile u16*)0x03007FF8)
#define ISR             *(void* volatile*)0x03007FFC
#define INT_VBLANK      (1 << 0)
#define INT_HBLANK      (1 << 1)
#define INT_VCOUNT      (1 << 2)
#define INT_TIMER0      (1 << 3)
#define INT_TIMER1      (1 << 4)
#define INT_TIMER2      (1 << 5)
#define INT_TIMER3      (1 << 6)
#define INT_SERIAL      (1 << 7)
#define INT_DMA0        (1 << 8)
#define INT_DMA1        (1 << 9)
#define INT_DMA2        (1 << 10)
#define INT_DMA3        (1 << 11)
#define INT_KEYPAD      (1 << 12)
#define INT_GAMEPAK     (1 << 13)
