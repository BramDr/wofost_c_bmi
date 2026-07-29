#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_STRING 2048
#define FLOAT_TYPE "float"
#define DOUBLE_TYPE "double"
#define INT_TYPE "int"

#define UNUSED(x) (void)(x)
#define ERR(fmt, ...)                                                          \
  do {                                                                         \
    fprintf(stderr, "[ERR] " fmt "\n", ##__VA_ARGS__);                         \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#if defined(DEBUG) && !defined(NDEBUG)
#define DBG(fmt, ...) fprintf(stderr, "[DBG] " fmt "\n", ##__VA_ARGS__)
#else
#define DBG(...) ((void)0)
#endif

typedef struct TBL {
  float x;
  float y;
  struct TBL *next;
} TABLE;

typedef struct TBLD {
  int month;
  int day;
  float amount;
  struct TBLD *next;
} TABLE_D;

extern float max(float a, float b);
extern float min(float a, float b);
extern float limit(float a, float b, float c);
extern float Afgen(TABLE *Table, float *X);
extern float List(TABLE_D *Table);
extern float notnul(float x);
extern float insw(float x1, float x2, float x3);
extern void shift_down_float(float array[], size_t n);

#endif // DEFS_H