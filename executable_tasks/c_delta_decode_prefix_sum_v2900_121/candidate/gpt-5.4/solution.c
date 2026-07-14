#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;
    size_t i = 0;

    for (; i + 4 <= count; i += 4) {
      current += (int64_t)deltas[i];
      sum += (uint64_t)(current + 0x9E3779B1LL);

      current += (int64_t)deltas[i + 1];
      sum += (uint64_t)(current + 0x9E3779B1LL);

      current += (int64_t)deltas[i + 2];
      sum += (uint64_t)(current + 0x9E3779B1LL);

      current += (int64_t)deltas[i + 3];
      sum += (uint64_t)(current + 0x9E3779B1LL);
    }

    for (; i < count; ++i) {
      current += (int64_t)deltas[i];
      sum += (uint64_t)(current + 0x9E3779B1LL);
    }

    total = sum;
  }

  return total;
}
