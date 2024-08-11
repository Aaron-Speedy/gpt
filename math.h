#ifndef MATH_H
#define MATH_H

#include "ds.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

typedef struct {
  float *buf;
  int w, h;
  int x0, y0; // for kernels
} Matrix;

#define m_at(m, i, j) (m).buf[(i) + (j) * (m).w]

void matrix_init(Arena *al, Matrix *m);
void matrix_print(Matrix m);
void matrix_randomize(Matrix m, float min, float max);
void matrix_copy(Matrix src, Matrix dst);
void matrix_fill(Matrix m, float v);

float randf(float min, float max);
int randi(int min, int max);
int sign(int x);
void clampf(float *x, float min, float max);
void clampi(int *x, int min, int max);
int in_bounds(int x, int max);

float maxf(float x, float y);
float minf(float x, float y);

#ifdef MATH_IMPL
#define MATH_IMPL

#define DS_IMPL
#include "ds.h"

void matrix_init(Arena *al, Matrix *m) {
  m->buf = arena_alloc(al, m->w * m->h * sizeof(float));
  memset(m->buf, 0.0, m->w * m->h * sizeof(float));
}

void matrix_print(Matrix m) {
  for (int j = 0; j < m.h; j++) {
    for (int i = 0; i < m.w; i++) {
      printf("%.3f ", m_at(m, i, j));
    }
    printf("\n");
  }
}

void matrix_randomize(Matrix m, float min, float max) {
  assert(min <= max);
  for (int i = 0; i < m.w; i++) {
    for (int j = 0; j < m.h; j++) {
      m_at(m, i, j) = randf(min, max);
    }
  }
}

void matrix_fill(Matrix m, float v) {
  for (int i = 0; i < m.w; i++) {
    for (int j = 0; j < m.h; j++) {
      m_at(m, i, j) = v;
    }
  }
}

void matrix_copy(Matrix src, Matrix dst) {
  assert(src.w == dst.w);
  assert(src.h == dst.h);

  for (int i = 0; i < src.w; i++) {
    for (int j = 0; j < src.h; j++) {
      m_at(dst, i, j) = m_at(src, i, j);
    }
  }
}

float randf(float min, float max) {
  float scale = rand() / (float) RAND_MAX;
  return min + scale * ( max - min );
}

int randi(int min, int max) {
  return (rand() % (max - min + 1)) + min;
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

float maxf(float x, float y) {
  return x > y ? x : y;
}

float minf(float x, float y) {
  return x <= y ? x : y;
}
#endif

#endif
