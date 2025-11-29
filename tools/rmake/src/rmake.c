#include "../../inc/res.h"
#include <libgen.h>
#include <string.h>
#include <stdlib.h>

#include "manifest.h"
#include "huffman.h"

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

void EncodeItem(ResInfo *info)
{
  FILE *f = fopen(info->path, "r+b");
  if (!f) {
    fprintf(stderr, "Could not open \"%s\"\n", info->path);
    exit(1);
  }

  fseek(f, 0, SEEK_END);
  info->size = ftell(f);
  fseek(f, 0, SEEK_SET);

  u8 *data;

  switch (info->compressionType) {
  case LZ77:
    // info->size = LZ77Encode(data, info->size, dst);
    return;
  case Huffman:
    data = malloc(info->size);
    fread(data, info->size, 1, f);
    fclose(f);
    HuffmanEncode(data, info);
    free(data);
    return;
  case RunLength:
    // info->size = RunLengthEncode(data, info->size, dst);
    return;
  case SubFilter:
    Filter(data, info->size, info->compressionArg);
    info->data = data;
    return;
  default:
    data = malloc(info->size + sizeof(u32));
    *((u32*)data) = info->size << 8;
    fread(data + sizeof(u32), info->size, 1, f);
    fclose(f);
    info->data = data;
    return;
  }
}

ResFile *NewResFile(Manifest *manifest)
{
  u32 mapSize = sizeof(ResMapItem)*manifest->count;
  u32 length = sizeof(ResFile) + mapSize;
  ResFile *resFile = malloc(length);
  resFile->count = manifest->count;
  resFile->length = length;
  return resFile;
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Usage: rmake Manifest\n");
    return 1;
  }

  Manifest *manifest = ParseManifest(argv[1]);

  for (u32 i = 0; i < manifest->count; i++) {
    EncodeItem(&manifest->items[i]);
  }

  ResFile *resFile = NewResFile(manifest);
  for (u32 i = 0; i < manifest->count; i++) {
    resFile->resMap[i].id = manifest->items[i].id;
    resFile->resMap[i].offset = resFile->length;
    resFile->length += Align(manifest->items[i].size, 4);
  }
  fwrite(resFile, sizeof(ResFile) + sizeof(ResMapItem)*manifest->count, 1, stdout);

  for (u32 i = 0; i < manifest->count; i++) {
    u32 zero = 0;
    u32 padSize = Align(manifest->items[i].size, 4) - manifest->items[i].size;

    fwrite(manifest->items[i].data, manifest->items[i].size, 1, stdout);
    fwrite(&zero, padSize, 1, stdout);
  }

  return 0;
}
