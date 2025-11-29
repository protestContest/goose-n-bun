#pragma once

typedef struct {
  char *name;
  u32 id;
  u32 compressionType;
  u32 compressionArg;
  char *path;
  u32 size;
  u8 *data;
} ResInfo;

typedef struct {
  char *path;
  u32 count;
  u32 capacity;
  ResInfo *items;
} Manifest;

Manifest *ParseManifest(char *path);
