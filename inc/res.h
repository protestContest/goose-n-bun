#pragma once

/*
Binary resources are embedded in the ROM by including them in a resource file. The resource file is
generated at build time from all files in the "res" folder by the `rmake` command. This is the
format of the resource file:

     +---------------+
     | numResources  |
     | totalLength   |
     +===============+ -+
     | res1 ID       |  |
  +-╴| res1 offset   |  |
  |  +---------------+  +- Resource map
  |  | res2 ID       |  |
+-|--| res2 offset   |  |
| |  +===============+ -+
| +->| res1 data     |
|    /               /
|    +---------------+
+-- >| res2 data     |
     /               /
     +---------------+

The `offset` properties in the resource map are offsets from the beginning of the resource file. The
resource IDs are hashes of the resource file names.
*/

typedef struct {
  u32 id;
  u32 offset;
} ResMapItem;

typedef struct {
  u32 numResources;
  u32 length;
  ResMapItem resMap[];
  /* resource data follows resMap */
} ResFile;

void *GetResource(char *name);
