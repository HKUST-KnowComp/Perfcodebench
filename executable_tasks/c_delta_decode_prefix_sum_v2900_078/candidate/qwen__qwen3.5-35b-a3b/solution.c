#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  int64_t* values = (int64_t*)malloc(count * sizeof(int64_t));
  if (!values) return 0;

  // Compute prefix sums once
  int64_t current = 0;
  for (size_t i = 0; i < count; ++i) {
    current += deltas[i];
    values[i] = current;
  }

  // Compute sum of (values[i] + constant) once
  uint64_t sum_once = 0;
  for (size_t i = 0; i < count; ++i) {
    sum_once += (uint64_t)(values[i] + 0x9E3779B1LL);
  }

  total = sum_once * iters;
  free(values);
  return total;
}