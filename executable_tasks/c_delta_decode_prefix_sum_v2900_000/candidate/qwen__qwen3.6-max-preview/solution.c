#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum_d = 0;
    uint64_t sum_d_idx = 0;
    for (size_t i = 0; i < count; ++i) {
      uint64_t d = (uint64_t)(int64_t)deltas[i];
      sum_d += d;
      sum_d_idx += d * (uint64_t)i;
    }
    uint64_t n = (uint64_t)count;
    total = n * 0x9E3779B1ULL + n * sum_d - sum_d_idx;
  }
  return total;
}