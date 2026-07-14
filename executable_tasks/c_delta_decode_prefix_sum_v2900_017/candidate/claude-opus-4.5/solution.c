#include "interface.h"

#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const int64_t offset = 0x9E3779B1LL;
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    size_t i = 0;
    size_t limit = count & ~(size_t)3;
    for (; i < limit; i += 4) {
      int64_t d0 = (int64_t)deltas[i];
      int64_t d1 = (int64_t)deltas[i + 1];
      int64_t d2 = (int64_t)deltas[i + 2];
      int64_t d3 = (int64_t)deltas[i + 3];
      int64_t v0 = current + d0;
      int64_t v1 = v0 + d1;
      int64_t v2 = v1 + d2;
      int64_t v3 = v2 + d3;
      sum0 += (uint64_t)(v0 + offset);
      sum1 += (uint64_t)(v1 + offset);
      sum2 += (uint64_t)(v2 + offset);
      sum3 += (uint64_t)(v3 + offset);
      current = v3;
    }
    for (; i < count; ++i) {
      current += (int64_t)deltas[i];
      sum0 += (uint64_t)(current + offset);
    }
    total = sum0 + sum1 + sum2 + sum3;
  }
  return total;
}