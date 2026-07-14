#include "interface.h"

#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const int64_t offset = 0x9E3779B1LL;
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;
    size_t i = 0;
    size_t limit = count & ~(size_t)3;
    for (; i < limit; i += 4) {
      current += (int64_t)deltas[i];
      sum += (uint64_t)(current + offset);
      current += (int64_t)deltas[i + 1];
      sum += (uint64_t)(current + offset);
      current += (int64_t)deltas[i + 2];
      sum += (uint64_t)(current + offset);
      current += (int64_t)deltas[i + 3];
      sum += (uint64_t)(current + offset);
    }
    for (; i < count; ++i) {
      current += (int64_t)deltas[i];
      sum += (uint64_t)(current + offset);
    }
    total = sum;
  }
  return total;
}
