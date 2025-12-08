#include "manifest.h"
#include "../../inc/kit/res.h"
#include <string.h>
#include <libgen.h>

#define IsWhitespace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n')

static u32 Hash(void *data, u32 size)
{
  u32 hash = 5381;
  u32 i;
  u8 *bytes = (u8*)data;
  for (i = 0; i < size; i++) {
    hash = ((hash << 5) + hash) + bytes[i];
  }
  return hash;
}

static void ManifestAppend(Manifest *manifest, ResInfo info)
{
  if (!manifest->capacity) {
    manifest->capacity = 4;
    manifest->items = realloc(manifest->items, sizeof(ResInfo)*manifest->capacity);
  } else if (manifest->capacity < manifest->count + 1) {
    manifest->capacity *= 2;
    manifest->items = realloc(manifest->items, sizeof(ResInfo)*manifest->capacity);
  }

  manifest->items[manifest->count++] = info;
}

static char *SkipWhitespace(char *cur, char *end)
{
  while (cur < end && IsWhitespace(*cur)) cur++;
  return cur;
}

static char *ParseName(char **cur, char *end)
{
  char *start = *cur;
  while (*cur < end && !IsWhitespace(**cur)) (*cur)++;
  u32 len = *cur - start;
  char *name = malloc(len + 1);
  strncpy(name, start, len);
  name[len] = 0;
  return name;
}

static char *PathJoin(char *dir, char *base)
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

static char *ReadManifest(char *path, u32 *size)
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

static u32 FileSize(char *path)
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

Manifest *ParseManifest(char *path)
{
  Manifest *manifest = malloc(sizeof(Manifest));
  char *pathCopy = strdup(path);
  manifest->path = dirname(pathCopy); // resource names are relative to the manifest path
  manifest->count = 0;
  manifest->capacity = 0;
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
    info.id = Hash(info.name, strlen(info.name));

    while (cur < fileEnd && IsWhitespace(*cur) && *cur != '\n') cur++;

    if (cur < fileEnd && *cur != '\n') {
      while (cur < fileEnd && !IsWhitespace(*cur)) {
        switch (*cur) {
        case 'L':
          info.compressionType = LZSS;
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
