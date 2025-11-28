#pragma once

// BIOS

typedef struct {
  u16 srcLength;
  u8 srcBits; // 1, 2, 4, or 8
  u8 dstBits; // 1, 2, 4, 8, 16, or 32
  u32 dataOffset; // bit 31 is zero flag
} UnpackInfo;
extern void BitUnPack(void *src, void *dst, UnpackInfo *info);

/*
src is a pointer to a 32-bit header, followed by data.
Header format:
  Bit 0-3   Data size (must be 1 for Diff8bit, 2 for Diff16bit)
  Bit 4-7   Type (must be 8 for DiffFiltered)
  Bit 8-31  24bit size after decompression
*/
extern void Diff8bitUnFilter(void *src, void *dst);
extern void Diff16bitUnFilter(void *src, void *dst);

typedef struct {
  u32 header;
    // Bit0-3   Data size in bit units (normally 4 or 8)
    // Bit4-7   Compressed type (must be 2 for Huffman)
    // Bit8-31  24bit size of decompressed data in bytes
  u8 treeSize; // Bit0-7   Size of Tree Table/2-1 (ie. Offset to Compressed Bitstream)
  u8 nodes[];
    // Root Node and Non-Data-Child Nodes are:
    //   Bit0-5  Offset to next child node,
    //           Next child node0 is at (CurrentAddr AND NOT 1)+Offset*2+2
    //           Next child node1 is at (CurrentAddr AND NOT 1)+Offset*2+2+1
    //   Bit6    Node1 End Flag (1=Next child node is data)
    //   Bit7    Node0 End Flag (1=Next child node is data)
    // Data nodes are (when End Flag was set in parent node):
    //   Bit0-7  Data (upper bits should be zero if Data Size is less than 8)
  // u32 bitstream[]; // Bit0-31  Node Bits (Bit31=First Bit)  (0=Node0, 1=Node1)
} HuffData;
extern void HuffUnComp(HuffData *src, void *dst);

typedef struct {
  u32 header;
    // Bit 0-3   Reserved (0)
    // Bit 4-7   Compressed type (must be 1 for LZ77)
    // Bit 8-31  Size of decompressed data
  struct {
    u8 blockTypes; // type of each block, MSB-first
    u8 blocks[8];
      // Block Type 0 - Uncompressed - Copy 1 Byte from Source to Dest
      //   Bit 0-7   One data byte to be copied to dest
      // Block Type 1 - Compressed - Copy N+3 Bytes from Dest-Disp-1 to Dest
      //   Bit 0-3   Disp MSBs
      //   Bit 4-7   Number of bytes to copy (minus 3)
      //   Bit 8-15  Disp LSBs
  } groups[];
} LZ77Data;
extern void LZ77UnComp(LZ77Data *src, void *dst);

typedef struct {
  u32 header;
    // Bit 0-3   Reserved
    // Bit 4-7   Compressed type (must be 3 for run-length)
    // Bit 8-31  Size of decompressed data
  struct {
    u8 flag;
      // Bit 0-6   Expanded Data Length (uncompressed N-1, compressed N-3)
      // Bit 7     Flag (0=uncompressed, 1=compressed)
    u8 data[];  // N uncompressed bytes, or 1 byte repeated N times
  } data[];
} RLData;
extern void RLUnComp(RLData *src, void *dst);
