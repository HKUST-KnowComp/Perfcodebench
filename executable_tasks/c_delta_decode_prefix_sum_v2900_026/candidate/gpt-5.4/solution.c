#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const uint64_t bias = 0x9E3779B1ULL;
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;

    size_t i = 0;
    size_t n4 = count & ~(size_t)3;
    for (; i < n4; i += 4) {
      current += (int64_t)deltas[i];
      sum += (uint64_t)(current + (int64_t)bias);

      current += (int64_t)deltas[i + 1];
      sum += (uint64_t)(current + (int64_t)bias);

      current += (int64_t)deltas[i + 2];
      sum += (uint64_t)(current + (int64_t)bias);

      current += (int64_t)deltas[i + 3];
      sum += (uint64_t)(current + (int64_t)bias);
    }

    for (; i < count; ++i) {
      current += (int64_t)deltas[i];
      sum += (uint64_t)(current + (int64_t)bias);
    }

    total = sum;
  }

  return total;
}
