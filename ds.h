#ifndef DS_H
#define DS_H

#ifndef CUSTOM_ASSERT
#include <assert.h>
#define CUSTOM_ASSERT(c) assert(c)
#endif

#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define arrlen(...) (size_t)(sizeof(__VA_ARGS__) / sizeof(*__VA_ARGS__))

typedef struct {
  char *buf;
  size_t cap, pos;
} Arena;

Arena arena_init(size_t cap);
void *arena_alloc(Arena *al, size_t len);

#ifdef DS_IMPL
#define DS_IMPL

Arena arena_init(size_t cap) {
  CUSTOM_ASSERT(cap > 0);

  Arena al = { .cap = cap, };
  al.buf = malloc(al.cap);

  return al;
}

void *arena_alloc(Arena *al, size_t len) {
  CUSTOM_ASSERT(al->pos + len <= al->cap && "Not enough memory in arena");
  CUSTOM_ASSERT(len > 0);

  void *mem = &al->buf[al->pos];
  al->pos += len;

  return mem;
}

#endif

#endif
