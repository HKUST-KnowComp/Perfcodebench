#include "interface.h"

#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t sum_per_iter = 0;
  int64_t current = 0;
  for (size_t i = 0; i < count; ++i) {
    current += deltas[i];
    sum_per_iter += (uint64_t)(current + 0x9E3779B1LL);
  }
  return sum_per_iter * iters;
}