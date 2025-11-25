#include "res.h"
#include "math.h"
#include "str.h"

extern const ResFile __Resources__;
#define resFile (&__Resources__)

void *GetResource(char *name)
{
  u32 id = Hash(name, StrLen(name));

  for (u32 i = 0; i < resFile->numResources; i++) {
    if (resFile->resMap[i].id == id) {
      return ((u8*)resFile) + resFile->resMap[i].offset;
    }
  }

  return 0;
}
