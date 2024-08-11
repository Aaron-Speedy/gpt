#ifndef UTILS_H
#define UTILS_H

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

#include <stdint.h>

typedef struct {
  int x, y;
} Vec2I;

typedef struct {
  int x, y;
} Vec2;

typedef struct {
  char *str;
  size_t cap, len;
} String;

float randf(float min, float max);
int randi(int min, int max);
int sign(int x);
void clampf(float *x, float min, float max);
void clampi(int *x, int min, int max);
int in_bounds(int x, int max);
int vec2i_at(Vec2I vec, int index);
String read_file(const char *path);

#ifdef UTILS_IMPL
#define UTILS_IMPL

#include <stdlib.h>
#include <assert.h>

#define List(T) struct { T *items; size_t cap, len; }

#define da_push(xs, x) \
  do { \
    if ((xs)->len >= (xs)->cap) { \
      if ((xs)->cap == 0) (xs)->cap = 256; \
      else (xs)->cap *= 2; \
      (xs)->items = realloc((xs)->items, (xs)->cap*sizeof(*(xs)->items)); \
    } \
 \
    (xs)->items[(xs)->len++] = (x); \
  } while (0)

#define da_pop(xs) \
  do { \
    assert ((xs)->len > 0); \
    (xs)->len -= 1; \
  } while (0)

#define da_last(xs) (assert((xs)->len > 0), (xs)->items[(xs)->len - 1])

#define da_init(xs) \
do { \
  (xs)->items = malloc((xs)->cap * sizeof((xs)->items[0])); \
} while (0);

float randf(float min, float max) {
  float scale = rand() / (float) RAND_MAX;
  return min + scale * ( max - min );
}

int randi(int min, int max) {
  return (rand() % (max - min)) + min + 1;
}

int sign(int x) {
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

void clampf(float *x, float min, float max) {
  if (*x < min) *x = min;
  else if (*x > max) *x = max;
}

void clampi(int *x, int min, int max) {
  if (*x < min) *x = min;
  else if (*x > max) *x = max;
}

int in_bounds(int x, int max) {
  if (x < 0) return 0;
  if (x >= max) return 0;
  return 1;
}

int in_bounds_v2i(Vec2I v, int max_x, int max_y) {
  if (!in_bounds(v.x, max_x)) return 0;
  if (!in_bounds(v.y, max_y)) return 0;
  return 1;
}

int vec2i_at(Vec2I vec, int index) {
  switch (index) {
    case 0:
      return vec.x;
    case 1:
      return vec.y;
    default:
      assert(!"Out of bounds");
  }
}

String read_file(const char *path) {
  FILE *f = fopen(path, "rb");
  assert(f != NULL && "Error: No such path\n");
  fseek(f, 0, SEEK_END);
  long m = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *str = malloc(m);
  fread(str, m, 1, f);

  return (String) { .str = str, .cap = m, .len = m, };
}

#endif

#endif
