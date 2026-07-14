#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  int64_t* values = (int64_t*)malloc(count * sizeof(int64_t));
  if (!values) return 0;
  const uint64_t base = 0x9E3779B1ULL;
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    for (size_t i = 0; i < count; ++i) {
      current += (int64_t)deltas[i];
      int64_t v = current;
      values[i] = v;
      total += (uint64_t)v + base;
    }
  }
  free(values);
  return total;
}