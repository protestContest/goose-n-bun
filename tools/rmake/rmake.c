#include "../../inc/prefix.h"
#include "../../inc/res.h"
#include <libgen.h>
#include <string.h>

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

int main(int argc, char *argv[])
{
  u32 numResources = 0;
  u32 dataSize = 0;

  for (u32 i = 1; i < argc; i++) {
    char *path = argv[i];
    FILE *f = fopen(path, "r+b");
    if (!f) {
      fprintf(stderr, "Could not find \"%s\"\n", path);
      continue;
    }
    fseek(f, 0, SEEK_END);
    u32 size = ftell(f);
    fclose(f);

    if (size > 0) {
      dataSize += size;
      numResources++;
    }
  }

  fprintf(stderr, "%d resources, %d bytes\n", numResources, dataSize);

  u32 mapSize = sizeof(ResMapItem)*numResources;
  u32 fileSize = sizeof(ResFile) + mapSize + dataSize;
  ResFile *resFile = malloc(fileSize);
  resFile->numResources = numResources;
  resFile->length = sizeof(ResFile) + mapSize + dataSize;
  u8 *data = ((u8*)resFile) + sizeof(ResFile) + mapSize;

  u32 j = 0;
  for (u32 i = 1; i < argc; i++) {
    char *path = argv[i];
    FILE *f = fopen(path, "r+b");
    if (!f) continue;
    fseek(f, 0, SEEK_END);
    u32 size = ftell(f);
    if (size == 0) {
      fclose(f);
      continue;
    }

    char *resName = basename(path);
    resFile->resMap[j].id = Hash(resName, strlen(resName));
    resFile->resMap[j].offset = data - ((u8*)resFile);

    fprintf(stderr, "Resource \"%s\" %08X: %d bytes at %d\n",
      resName,
      resFile->resMap[j].id,
      size,
      resFile->resMap[j].offset);

    fseek(f, 0, SEEK_SET);
    fread(data, size, 1, f);
    data += size;
    fclose(f);
    j++;
  }

  fprintf(stderr, "Writing res file %d bytes\n", fileSize);

  fwrite(resFile, fileSize, 1, stdout);

  return 0;
}
