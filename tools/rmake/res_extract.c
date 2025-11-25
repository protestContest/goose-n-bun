#include "../../inc/prefix.h"
#include <libgen.h>
#include <string.h>

typedef struct {
  u32 dataOffset;
  u32 mapOffset;
  u32 dataLength;
  u32 mapLength;
  u8 reserved[112];
  u8 appData[128];
} ResFile;

typedef struct {
  u8 reserved[22];
  u16 fileAttrs;
  u16 typeListOffset;
  u16 nameListOffset;
} ResMap;

typedef struct {
  u32 type;
  u16 numResources;
  u16 refListOffset;
} ResTypeData;

typedef struct __attribute__((packed)) {
  u16 numTypes;
  ResTypeData items[];
} ResTypeList;

typedef struct {
  i16 resID;
  u16 nameOffset;
  u32 dataOffset;
  u8 reserved[4];
} ResRef;

typedef struct {
  i16 type;
  i16 first_char;
  i16 last_char;
  i16 wid_max;
  i16 kern_max;
  i16 neg_descent;
  i16 rect_width;
  i16 rect_height;
  i16 owt_offset;
  i16 ascent;
  i16 descent;
  i16 leading;
  i16 row_words;
} FontRec;

#define IsDigit(c)  ((c) >= '0' && (c) <= '9')

u32 SwapLong(u32 n)
{
  return ((n & 0x000000FF) << 24) | ((n & 0x0000FF00) << 8) | ((n & 0x00FF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

u16 SwapShort(u16 n)
{
  return (n << 8) | (n >> 8);
}

void *GetResource(u32 type, i16 resID, ResFile *resFile)
{
  u8 *resData = ((u8*)resFile) + SwapLong(resFile->dataOffset);
  ResMap *resMap = (ResMap*)(((u8*)resFile) + SwapLong(resFile->mapOffset));
  u16 typeListOffset = SwapShort(resMap->typeListOffset);

  ResTypeList *typeList = (ResTypeList*)(((u8*)resMap) + typeListOffset);

  u16 numTypes = SwapShort(typeList->numTypes) + 1;

  for (u16 i = 0; i < numTypes; i++) {
    if (SwapLong(typeList->items[i].type) == type) {
      u16 refListOffset = SwapShort(typeList->items[i].refListOffset);

      ResRef *refs = (ResRef*)(((u8*)typeList) + refListOffset);
      u16 numResources = SwapShort(typeList->items[i].numResources) + 1;

      for (u16 j = 0; j < numResources; j++) {
        if (SwapShort(refs[j].resID) == resID) {
          u32 offset = SwapLong(refs[j].dataOffset) & 0xFFFFFF;
          return resData + offset;
        }
      }

      return 0;
    }
  }

  return 0;
}

void FixFont(FontRec *rec)
{
  i32 i, table_size;
  i16 *bit_image, *loc_table, *ow_table;

  for (i = 0; i < 13; i++) {
    ((i16*)rec)[i] = SwapShort(((i16*)rec)[i]);
  }

  table_size = rec->last_char - rec->first_char + 2;
  bit_image = (i16*)(rec + 1);
  loc_table = bit_image + rec->row_words*rec->rect_height;
  ow_table = &rec->owt_offset + rec->owt_offset;

  for (i = 0; i < rec->row_words*rec->rect_height; i++) {
    bit_image[i] = SwapShort(bit_image[i]);
  }
  for (i = 0; i < table_size+1; i++) {
    loc_table[i] = SwapShort(loc_table[i]);
  }
  for (i = 0; i < table_size; i++) {
    ow_table[i] = SwapShort(ow_table[i]);
  }
}

int main(int argc, char *argv[])
{
  if (argc < 4) {
    fprintf(stderr, "Usage: res-extract <resFile> <type> <resID>\n");
    return 1;
  }

  char *resPath = argv[1];
  u32 type = (argv[2][0] << 24) | (argv[2][1] << 16) | (argv[2][2] << 8) | argv[2][3];
  u16 resID = 0;

  char *idStr = argv[3];
  while (*idStr && IsDigit(*idStr)) {
    u16 digit = (*idStr) - '0';
    resID = resID*10 + digit;
    idStr++;
  }

  FILE *f = fopen(resPath, "r+b");
  if (!f) {
    fprintf(stderr, "File not found\n");
    return 2;
  }

  fseek(f, 0, SEEK_END);
  u32 size = ftell(f);
  fseek(f, 0, SEEK_SET);

  ResFile *resFile = malloc(size);
  fread(resFile, size, 1, f);
  fclose(f);

  u8 *resource = GetResource(type, resID, resFile);
  if (!resource) {
    fprintf(stderr, "Resource %s %d not found\n", argv[2], resID);
    return 3;
  }
  u32 resSize = SwapLong(*((u32*)resource));
  u8 *data = resource + sizeof(resSize);

  if (type == 'FONT') {
    FixFont((FontRec*)data);
  }

  fwrite(data, resSize, 1, stdout);

  return 0;
}
