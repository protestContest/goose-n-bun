#include "../../inc/prefix.h"
#include "../../inc/res.h"
#include <libgen.h>
#include <string.h>
#include <stdlib.h>

/*
Reads a manifest file and packs resources into a resource file. The manifest file is a list of
resource specs, one per line. Each resource spec specifies the relative path of the resource
followed by a list of optional flag characters:

  L: Compress with LZ77
  h: Compress with Huffman, 4-bit
  H: Compress with Huffman, 8-bit
  R: Compress with run-length
  f: Filter 8-bit chunks before compressing
  F: Filter 16-bit chunks before compressing

*/

typedef struct {
  char *name;
  u32 compressionType;
  u32 compressionArg;
  char *path;
  u32 size;
} ResInfo;

typedef struct {
  char *path;
  u32 count;
  u32 capacity;
  u32 dataSize;
  ResInfo *items;
} Manifest;

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

#define IsWhitespace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n')
#define Align(n, m)       (((u64)(n) + (u64)(m) - 1) & ~((u64)(m) - 1))

// add to dynamic array, and adjust dataSize
void ManifestAppend(Manifest *manifest, ResInfo info)
{
  if (!manifest->capacity) {
    manifest->capacity = 4;
    manifest->items = realloc(manifest->items, sizeof(ResInfo)*manifest->capacity);
  } else if (manifest->capacity < manifest->count + 1) {
    manifest->capacity *= 2;
    manifest->items = realloc(manifest->items, sizeof(ResInfo)*manifest->capacity);
  }

  manifest->items[manifest->count++] = info;
  manifest->dataSize += Align(info.size, 4);
}

char *ReadManifest(char *path, u32 *size)
{
  FILE *mFile = fopen(path, "r");
  if (!mFile) {
    fprintf(stderr, "Could not open manifest \"%s\"\n", path);
    exit(1);
  }

  fseek(mFile, 0, SEEK_END);
  *size = ftell(mFile);

  char *contents = malloc(*size);

  fseek(mFile, 0, SEEK_SET);
  fread(contents, *size, 1, mFile);
  fclose(mFile);

  return contents;
}

char *SkipWhitespace(char *cur, char *end)
{
  while (cur < end && IsWhitespace(*cur)) cur++;
  return cur;
}

char *ParseName(char **cur, char *end)
{
  char *start = *cur;
  while (*cur < end && !IsWhitespace(**cur)) (*cur)++;
  u32 len = *cur - start;
  char *name = malloc(len + 1);
  strncpy(name, start, len);
  name[len] = 0;
  return name;
}

char *PathJoin(char *dir, char *base)
{
  u32 dirLen = strlen(dir);
  u32 baseLen = strlen(base);
  u32 len = dirLen + baseLen + 1;
  char *path = malloc(len + 1);
  path[len] = 0;
  strncpy(path, dir, dirLen);
  path[dirLen] = '/';
  strncpy(path + dirLen + 1, base, baseLen);
  return path;
}

u32 FileSize(char *path)
{
  FILE *file = fopen(path, "r+b");
  if (!file) {
    fprintf(stderr, "Could not open resource \"%s\"\n", path);
    exit(2);
  }

  fseek(file, 0, SEEK_END);
  u32 size = ftell(file);
  fclose(file);
  return size;
}

// parses a manifest file
Manifest *ParseManifest(char *path)
{
  Manifest *manifest = malloc(sizeof(Manifest));
  char *pathCopy = strdup(path);
  manifest->path = dirname(pathCopy); // resource names are relative to the manifest path
  manifest->count = 0;
  manifest->capacity = 0;
  manifest->dataSize = 0;
  manifest->items = 0;

  u32 mSize;
  char *contents = ReadManifest(path, &mSize);

  // parse the file
  char *fileEnd = contents + mSize;
  char *cur = contents;
  while (cur < fileEnd) {
    ResInfo info = {0};

    // find the first character (resource name)
    cur = SkipWhitespace(cur, fileEnd);
    if (cur == fileEnd) break;

    info.name = ParseName(&cur, fileEnd);

    while (cur < fileEnd && IsWhitespace(*cur) && *cur != '\n') cur++;

    if (cur < fileEnd && *cur != '\n') {
      while (cur < fileEnd && !IsWhitespace(*cur)) {
        switch (*cur) {
        case 'L':
          info.compressionType = LZ77;
          break;
        case 'h':
          info.compressionType = Huffman;
          info.compressionArg = 4;
          break;
        case 'H':
          info.compressionType = Huffman;
          info.compressionArg = 8;
          break;
        case 'R':
          info.compressionType = RunLength;
          break;
        case 'f':
          info.compressionType = SubFilter;
          info.compressionArg = 1;
          break;
        case 'F':
          info.compressionType = SubFilter;
          info.compressionArg = 2;
          break;
        }
        cur++;
      }
    }

    info.path = PathJoin(manifest->path, info.name);
    info.size = FileSize(info.path);

    if (info.size == 0) {
      fprintf(stderr, "Skipping empty resource \"%s\"\n", info.path);
      free(info.path);
      free(info.name);
    } else {
      ManifestAppend(manifest, info);
    }
  }

  free(contents);

  return manifest;
}

void Filter(u8 *data, u32 size, u32 dataSize)
{
  if (dataSize == 2) {
    i16 *words = (i16*)data;
    size >>= 1;
    i16 prev = words[0];
    for (u32 i = 1; i < size; i++) {
      i16 cur = words[i];
      words[i] = cur - prev;
      prev = cur;
    }
  } else {
    i8 *bytes = (i8*)data;
    i8 prev = bytes[0];
    for (u32 i = 1; i < size; i++) {
      i8 cur = bytes[i];
      bytes[i] = cur - prev;
      prev = cur;
    }
  }
}

u32 HuffmanEncode(u8 *data, u32 size, u32 bits, u8 *dst)
{
  fprintf(stderr, "Unimplemented\n");
  exit(3);
  return size;
}

u32 LZ77Encode(u8 *data, u32 size, u8 *dst)
{
  fprintf(stderr, "Unimplemented\n");
  exit(3);
  return size;
}

u32 RunLengthEncode(u8 *data, u32 size, u8 *dst)
{
  fprintf(stderr, "Unimplemented\n");
  exit(3);
  return size;
}

u8 *CopyItem(ResInfo *info, FILE *f, u8 *dst)
{
  fseek(f, 0, SEEK_SET);

  u32 flags = (info->size << 8) | (info->compressionType << 4) | (info->compressionArg);
  *((u32*)dst) = flags;
  dst += sizeof(flags);

  if (info->compressionType == 0) {
    fread(dst, info->size, 1, f);
    return dst + Align(info->size, 4);
  }

  u8 *data = malloc(info->size);
  fread(data, info->size, 1, f);

  switch (info->compressionType) {
  case LZ77:
    info->size = LZ77Encode(data, info->size, dst);
    break;
  case Huffman:
    info->size = HuffmanEncode(data, info->size, info->compressionArg, dst);
    break;
  case RunLength:
    info->size = RunLengthEncode(data, info->size, dst);
    break;
  case SubFilter:
    memcpy(dst, data, info->size);
    Filter(dst, info->size, info->compressionArg);
    break;
  default:
    memcpy(dst, data, info->size);
  }

  free(data);

  fprintf(stderr, "Next: %lld\n", (u64)dst + Align(info->size, 4));

  return dst + Align(info->size, 4);
}

ResFile *NewResFile(Manifest *manifest)
{
  u32 mapSize = sizeof(ResMapItem)*manifest->count;
  u32 fileSize = sizeof(ResFile) + mapSize + manifest->dataSize;
  ResFile *resFile = malloc(fileSize);
  resFile->count = manifest->count;
  resFile->length = fileSize;
  return resFile;
}

#define ResFileData(resFile)  (((u8*)(resFile)) + sizeof(ResFile) + sizeof(ResMapItem)*(resFile)->count)

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Usage: rmake Manifest\n");
    return 1;
  }

  Manifest *manifest = ParseManifest(argv[1]);

  fprintf(stderr, "%d resources, %d bytes\n", manifest->count, manifest->dataSize);

  ResFile *resFile = NewResFile(manifest);
  u8 *data = ResFileData(resFile);
  u8 *start = data;

  for (u32 i = 0; i < manifest->count; i++) {
    char *path = manifest->items[i].path;
    FILE *f = fopen(path, "r+b");
    if (!f) {
      fprintf(stderr, "Could not open \"%s\"\n", path);
      exit(1);
    }

    char *resName = manifest->items[i].name;
    resFile->resMap[i].id = Hash(resName, strlen(resName));
    resFile->resMap[i].offset = data - ((u8*)resFile);

    data = CopyItem(&manifest->items[i], f, data);

    fprintf(stderr, "Copied %ld bytes\n", data - start);

    fclose(f);

    fprintf(stderr, "Resource \"%s\" %08X: %d bytes at %d\n",
      resName,
      resFile->resMap[i].id,
      manifest->items[i].size,
      resFile->resMap[i].offset);
  }

  fprintf(stderr, "Writing res file %d bytes\n", resFile->length);

  fwrite(resFile, resFile->length, 1, stdout);

  return 0;
}
