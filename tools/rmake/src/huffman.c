#include "huffman.h"

typedef struct HuffNode {
  u8 data;
  u32 weight;
  struct HuffNode *left;
  struct HuffNode *right;
  struct HuffNode *next;
} HuffNode;

typedef struct {
  u32 code;
  u8 length;
} HuffDict;

#define IsLeaf(node)  (!(node)->left &&  !(node)->right)

static HuffNode *InsertNode(HuffNode *queue, HuffNode *node)
{
  if (!queue) return node;

  HuffNode *next = queue;
  HuffNode *prev = 0;
  while (next && next->weight < node->weight) {
    prev = next;
    next = next->next;
  }

  node->next = next;
  if (prev) {
    prev->next = node;
    return queue;
  } else {
    return node;
  }
}

static void AppendNode(HuffNode *queue, HuffNode *node)
{
  while (queue->next) {
    queue = queue->next;
  }
  queue->next = node;
}

static u8 *WriteHuffTree(HuffNode *node, u8 *end, u8 *dst)
{
  if (!node) return dst;

  if (IsLeaf(node)) {
    *dst++ = node->data;
    return WriteHuffTree(node->next, end, dst);
  } else {
    // write byte
    u32 offset = end - dst;
    u8 byte = (Align(offset, 2) - 2)/2;
    if (IsLeaf(node->right)) {
      byte |= (1 << 6);
    }
    if (IsLeaf(node->left)) {
      byte |= (1 << 7);
    }
    *dst++ = byte;

    node->left->next = node->right;

    AppendNode(node, node->left);

    return WriteHuffTree(node->next, end+2, dst);
  }
}

static void MakeHuffDict(HuffNode *tree, HuffDict *dict, u32 code, u8 length)
{
  if (IsLeaf(tree)) {
    dict[tree->data].code = code;
    dict[tree->data].length = length;
  } else {
    MakeHuffDict(tree->left, dict, code << 1, length + 1);
    MakeHuffDict(tree->right, dict, (code << 1) + 1, length + 1);
  }
}

static u32 HuffSize(u8 *data, u32 length, HuffDict *dict)
{
  u32 bits = 0;
  for (u32 i = 0; i < length; i++) {
    u8 byte = data[i];
    bits += dict[byte].length;
  }
  return Align(bits, 8) / 8;
}

static u32 *WriteHuffData(u8 *data, u32 length, HuffDict *dict, u32 *dst)
{
  u32 bits = 0;

  for (u32 i = 0; i < length; i++) {
    u8 byte = data[i];
    u32 code = dict[byte].code;
    u8 len = dict[byte].length;


    for (u32 b = 0; b < len; b++) {
      u32 m = (1 << (len - b - 1));

      if (bits == 0) *dst = 0;
      if (m & code) {
        *dst |= (1 << (31-bits));
      }
      bits++;
      if (bits == 32) {
        bits = 0;
        dst++;
      }
    }
  }

  return dst;
}

void HuffmanEncode(u8 *data, ResInfo *info)
{
  u32 freq[256] = {0};
  HuffNode *tree = 0;
  u32 treeSize = 0;

  // count data frequency
  for (u32 i = 0; i < info->size; i++) {
    freq[data[i]]++;
  }

  // create node priority queue
  for (u32 i = 0; i < 256; i++) {
    if (freq[i] > 0) {
      HuffNode *node = malloc(sizeof(HuffNode));
      node->data = i;
      node->weight = freq[i];
      node->left = 0;
      node->right = 0;
      node->next = 0;
      tree = InsertNode(tree, node);
      treeSize++;
    }
  }

  // build huffman tree
  while (tree->next) {
    HuffNode *node = malloc(sizeof(HuffNode));
    node->data = 0;
    node->weight = tree->weight + tree->next->weight;
    node->left = tree;
    node->right = tree->next;
    node->next = 0;
    tree = InsertNode(tree->next->next, node);
    node->right->next = 0;
    treeSize++;
  }

  // convert tree to dictionary
  HuffDict dict[256] = {0};
  MakeHuffDict(tree, dict, 0, 0);

  // write data
  u32 dataSize = Align(HuffSize(data, info->size, dict), 4);
  u32 encSize =
    sizeof(u32) + // header
    sizeof(u8) +  // tree size byte
    treeSize +    // tree
    dataSize;     // data stream

  u8 *encoded = malloc(Align(encSize, 4));
  info->data = encoded;

  // header
  *((u32*)encoded) = (info->size << 8) | (info->compressionType << 4) | (info->compressionArg);
  encoded += sizeof(u32);

  // tree size byte
  *encoded++ = (treeSize - 1) >> 1;

  encoded = WriteHuffTree(tree, encoded+1, encoded);
  WriteHuffData(data, info->size, dict, (u32*)encoded);

  info->size = encSize;
}
