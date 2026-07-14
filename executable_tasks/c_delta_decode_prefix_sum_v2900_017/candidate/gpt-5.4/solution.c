#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t bias = 0x9E3779B1ULL;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;

    size_t i = 0;
    size_t n4 = count & ~(size_t)3;
    for (; i < n4; i += 4) {
      int64_t c0 = current + (int64_t)deltas[i];
      int64_t c1 = c0 + (int64_t)deltas[i + 1];
      int64_t c2 = c1 + (int64_t)deltas[i + 2];
      int64_t c3 = c2 + (int64_t)deltas[i + 3];

      sum += (uint64_t)c0 + bias;
      sum += (uint64_t)c1 + bias;
      sum += (uint64_t)c2 + bias;
      sum += (uint64_t)c3 + bias;

      current = c3;
    }

    for (; i < count; ++i) {
      current += (int64_t)deltas[i];
      sum += (uint64_t)current + bias;
    }

    total = sum;
  }

  return total;
}
