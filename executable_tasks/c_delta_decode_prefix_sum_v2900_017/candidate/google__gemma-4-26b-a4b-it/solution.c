#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  if (count == 0) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current_prefix = 0;
    uint64_t iter_total = 0;
    for (size_t i = 0; i < count; ++i) {
      current_prefix += (int64_t)deltas[i];
      iter_total += (uint64_t)(current_prefix + 0x9E3779B1LL);
    }
    total = iter_total;
  }
  return total;
}