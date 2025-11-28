#pragma once

/*
Binary resources are embedded in the ROM by including them in a resource file. The resource file is
generated at build time from all files in the "res" folder by the `rmake` command. This is the
format of the resource file:

     +---------------+
     | count         |
     | totalLength   |
     +===============+
     | res1 ID       |  \
  +-╴| res1 offset   |  |
  |  +---------------+  +- Resource map
  |  | res2 ID       |  |
+-|--| res2 offset   |  /
| |  +===============+
  +->| res1 header   |
|    | res1 data     |
|    /               /
|    +---------------+
+--->| res2 header   |
     | res2 data     |
     /               /
     +---------------+

The `offset` properties in the resource map are offsets from the beginning of the resource file. The
resource IDs are hashes of the resource file names.

Each resource begins with a header. The high 24 bits are the resource's uncompressed size. The low
8 bits are flags:

             7 654 32 10
            +-+---+--+--+
            | |   |  |  |
            +-+---+--+--+
             |   |  |  |
Filtered-----+   |  |  |
Compression type-+  |  |
Huffman size--------+  |
Filter size------------+

Bit 0: Filter data size 8-bit
Bit 1: Filter data size 16-bit
Bit 2: Huffman data size 4-bit
Bit 3: Huffman data size 8-bit
Bit 4-6: Compression type:
  LZ77: 1
  Huffman: 2
  Run length: 3
Bit 7: Set when data is filtered
*/

#define Filter8Bit      ((1 << 7) | (1 << 0))
#define Filter16Bit     ((1 << 7) | (1 << 1))
#define Huffman4Bit     ((2 << 4) | (1 << 2))
#define Huffman8Bit     ((2 << 4) | (1 << 3))
#define LZ77            (1 << 4)
#define RunLength       (3 << 4)

typedef struct {
  u32 id;
  u32 offset;
} ResMapItem;

typedef struct {
  u32 count;
  u32 length;
  ResMapItem resMap[];
  /* resource data follows resMap */
} ResFile;

void *GetResource(char *name);
