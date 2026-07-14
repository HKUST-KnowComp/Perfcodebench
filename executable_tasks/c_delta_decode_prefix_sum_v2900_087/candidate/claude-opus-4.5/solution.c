#include "interface.h"

#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const int64_t offset = 0x9E3779B1LL;
  const uint64_t total_offset = (uint64_t)offset * count;
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    int64_t sum = 0;
    size_t i = 0;
    size_t limit = count & ~(size_t)3;
    for (; i < limit; i += 4) {
      current += deltas[i];
      int64_t v0 = current;
      current += deltas[i + 1];
      int64_t v1 = current;
      current += deltas[i + 2];
      int64_t v2 = current;
      current += deltas[i + 3];
      int64_t v3 = current;
      sum += v0 + v1 + v2 + v3;
    }
    for (; i < count; ++i) {
      current += deltas[i];
      sum += current;
    }
    total = (uint64_t)sum + total_offset;
  }
  return total;
}