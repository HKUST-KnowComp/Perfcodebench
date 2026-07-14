#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  uint64_t constant_term = (uint64_t)count * 0x9E3779B1ULL;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t cur = 0;
    uint64_t sum_cur = 0;
    for (size_t i = 0; i < count; ++i) {
      cur += (int64_t)deltas[i];
      sum_cur += (uint64_t)cur;
    }
    total = sum_cur + constant_term;
  }
  return total;
}
