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

typedef struct {
  u32 entryID;
  u32 offset;
  u32 length;
} AppleDoubleEntry;

typedef struct __attribute__((packed)) {
  u32 tag;
  u32 version;
  u8 filler[16];
  u16 numEntries;
  AppleDoubleEntry entries[];
} AppleDouble;

#define AppleDoubleTag "\x00\x05\x16\x07"
ResFile *OpenFile(char *path)
{
  FILE *f = fopen(path, "r+b");
  if (!f) {
    fprintf(stderr, "File \"%s\" not found\n", path);
    exit(2);
  }

  fseek(f, 0, SEEK_END);
  u32 size = ftell(f);

  char tag[4];
  fseek(f, 0, SEEK_SET);
  fread(tag, 4, 1, f);
  if (strncmp(tag, AppleDoubleTag, 4) == 0) {
    AppleDouble *doubleFile = malloc(size);
    fseek(f, 0, SEEK_SET);
    fread(doubleFile, size, 1, f);
    fclose(f);

    for (u16 i = 0; i < SwapShort(doubleFile->numEntries); i++) {
      if (SwapLong(doubleFile->entries[i].entryID) == 2) {
        ResFile *resFile = (ResFile*)(((u8*)doubleFile) + SwapLong(doubleFile->entries[i].offset));
        return resFile;
      }
    }
    fprintf(stderr, "AppleDouble does not contain a resource fork\n");
    exit(3);
  } else {
    ResFile *resFile = malloc(size);
    fseek(f, 0, SEEK_SET);
    fread(resFile, size, 1, f);
    fclose(f);
    return resFile;
  }
}

u32 ParseType(char *str)
{
  if (strlen(str) != 4) {
    fprintf(stderr, "Invalid type \"%s\"", str);
    exit(2);
  }
  return (str[0] << 24) | (str[1] << 16) | (str[2] << 8) | str[3];
}

u32 ParseID(char *str)
{
  u16 resID = 0;
  while (*str && IsDigit(*str)) {
    u16 digit = (*str) - '0';
    resID = resID*10 + digit;
    str++;
  }
  return resID;
}

void PrintResourceInfo(ResRef *ref, char *type, u8 *resData, char *nameList)
{
  u32 dataOffset = SwapLong(ref->dataOffset) & 0xFFFFFF;
  u32 *data = (u32*)(resData + dataOffset);
  u32 length = SwapLong(data[0]);
  i16 nameOffset = SwapShort(ref->nameOffset);
  char nameStr[256] = {0};
  nameStr[0] = '-';

  if (nameOffset >= 0) {
    char *name = nameList + nameOffset;
    memcpy(nameStr, name+1, name[0]);
  }

  printf("%s %d \"%s\" (%d bytes)\n", type, SwapShort(ref->resID), nameStr, length);
}

void ListAllResources(ResFile *resFile)
{
  u8 *resData = ((u8*)resFile) + SwapLong(resFile->dataOffset);
  ResMap *resMap = (ResMap*)(((u8*)resFile) + SwapLong(resFile->mapOffset));
  u16 typeListOffset = SwapShort(resMap->typeListOffset);
  u16 nameListOffset = SwapShort(resMap->nameListOffset);
  char *nameList = ((char*)resMap) + nameListOffset;

  ResTypeList *typeList = (ResTypeList*)(((u8*)resMap) + typeListOffset);

  u16 numTypes = SwapShort(typeList->numTypes) + 1;

  for (u16 i = 0; i < numTypes; i++) {
    u32 type = SwapLong(typeList->items[i].type);
    char typeStr[5] = {0};
    typeStr[0] = type >> 24;
    typeStr[1] = (type >> 16) & 0xFF;
    typeStr[2] = (type >> 8) & 0xFF;
    typeStr[3] = type & 0xFF;

    u16 refListOffset = SwapShort(typeList->items[i].refListOffset);

    ResRef *refs = (ResRef*)(((u8*)typeList) + refListOffset);
    u16 numResources = SwapShort(typeList->items[i].numResources) + 1;

    for (u16 j = 0; j < numResources; j++) {
      PrintResourceInfo(&refs[j], typeStr, resData, nameList);
    }
  }
}

void ListResourcesOfType(ResFile *resFile, u32 type)
{
  u8 *resData = ((u8*)resFile) + SwapLong(resFile->dataOffset);
  ResMap *resMap = (ResMap*)(((u8*)resFile) + SwapLong(resFile->mapOffset));
  u16 typeListOffset = SwapShort(resMap->typeListOffset);
  u16 nameListOffset = SwapShort(resMap->nameListOffset);
  char *nameList = ((char*)resMap) + nameListOffset;

  char typeStr[5] = {0};
  typeStr[0] = type >> 24;
  typeStr[1] = (type >> 16) & 0xFF;
  typeStr[2] = (type >> 8) & 0xFF;
  typeStr[3] = type & 0xFF;

  ResTypeList *typeList = (ResTypeList*)(((u8*)resMap) + typeListOffset);

  u16 numTypes = SwapShort(typeList->numTypes) + 1;

  for (u16 i = 0; i < numTypes; i++) {
    if (SwapLong(typeList->items[i].type) == type) {
      u16 refListOffset = SwapShort(typeList->items[i].refListOffset);

      ResRef *refs = (ResRef*)(((u8*)typeList) + refListOffset);
      u16 numResources = SwapShort(typeList->items[i].numResources) + 1;

      for (u16 j = 0; j < numResources; j++) {
        PrintResourceInfo(&refs[j], typeStr, resData, nameList);
      }

      return;
    }
  }

  fprintf(stderr, "No resources of type %s found\n", typeStr);
}

void DumpResource(ResFile *resFile, u32 type, u32 resID)
{
  u8 *resource = GetResource(type, resID, resFile);
  if (!resource) {
    char typeStr[5] = {0};
    memcpy(&type, typeStr, 4);
    fprintf(stderr, "Resource %s %d not found\n", typeStr, resID);
    exit(3);
  }
  u32 resSize = SwapLong(*((u32*)resource));
  u8 *data = resource + sizeof(resSize);

  if (type == 'FONT') {
    FixFont((FontRec*)data);
  }

  fwrite(data, resSize, 1, stdout);
}

int main(int argc, char *argv[])
{
  if (argc == 2) {
    ListAllResources(OpenFile(argv[1]));
  } else if (argc == 3) {
    ListResourcesOfType(OpenFile(argv[1]), ParseType(argv[2]));
  } else if (argc == 4) {
    DumpResource(OpenFile(argv[1]), ParseType(argv[2]), ParseID(argv[3]));
  } else {
    fprintf(stderr, "Usage: res-extract <resFile> [<type>] [<resID>]\n");
    return 1;
  }

  return 0;
}
