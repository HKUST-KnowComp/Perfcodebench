#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  uint64_t const_term = (uint64_t)count * 0x9E3779B1LL;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sum_p = 0;
    for (size_t i = 0; i < count; ++i) {
      sum_p += (int64_t)deltas[i] * (int64_t)(count - i);
    }
    total = (uint64_t)sum_p + const_term;
  }
  return total;
}
