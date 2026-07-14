#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t weighted_sum = 0;
    for (size_t j = 0; j < count; ++j) {
      weighted_sum += (int64_t)deltas[j] * (int64_t)(count - j);
    }
    total = (uint64_t)weighted_sum + (uint64_t)count * 0x9E3779B1ULL;
  }
  return total;
}