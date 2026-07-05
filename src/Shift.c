#include "defs.h"
#include <stddef.h>

void shift_down_float(float array[], size_t n) {
  if (n < 2) {
    return;
  }

  float first = array[0];
  for (size_t i = 0; i < n - 1; i++) {
    array[i] = array[i + 1];
  }
  array[n - 1] = first;
}