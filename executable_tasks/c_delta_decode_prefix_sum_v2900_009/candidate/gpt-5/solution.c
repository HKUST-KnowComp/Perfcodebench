#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const int64_t K = 0x9E3779B1LL;
  const int32_t* p = deltas;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t t = 0;

    size_t i = 0;
    size_t n4 = count & ~(size_t)3;

    for (; i < n4; i += 4) {
      current += (int64_t)p[i];
      t += (uint64_t)(current + K);
      current += (int64_t)p[i + 1];
      t += (uint64_t)(current + K);
      current += (int64_t)p[i + 2];
      t += (uint64_t)(current + K);
      current += (int64_t)p[i + 3];
      t += (uint64_t)(current + K);
    }

    for (; i < count; ++i) {
      current += (int64_t)p[i];
      t += (uint64_t)(current + K);
    }

    total = t;
  }

  return total;
}
